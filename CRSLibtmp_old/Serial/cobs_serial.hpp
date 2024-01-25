// ROS1/2に非依存。
// Boost::asioを使う。
// ハンドシェイクはしない。このレイヤで受け持つ機能ではないだろう。

#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <memory>

#include <boost/asio.hpp>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/reporter.hpp>

#include "pack.hpp"

#define Strnize1(x) #x
#define Strnize2(x) Strnize1(x)

namespace CRSLib::Usb
{
	// COBS(Consistent Overhead Byte Stuffing)でシリアル通信を行う
	// read, write操作を1つずつ並列に実行できる。どちらか一方でも複数同時に行うことはできない
	// 送受信ともに失敗したら再度試みる。
	// **全く以てシグナル安全でない**
	template<reporter ErrorReporter = DefaultErrorReporter, reporter InfoReporter = DefaultInfoReporter>
	class CobsSerial final
	{
		// io_contextまわり。おそらく、いくつかのスレッドで並行に実行する処理をまとめたモノ
		boost::asio::io_context io{};

		// CTAD for aggregateはまだまだ時々コンパイルが通らない
		// read, write操作を要求されてないときにio.run()が止まらないようにする
		boost::asio::executor_work_guard<decltype(io.get_executor())> work_guard{io.get_executor()};

		// 同時に動く処理は高々2つなので
		boost::asio::thread_pool thread_pool{2};
	
		// serial_port。async操作は並列に実行できないこともあるので注意(ex. async_writeは並列に呼び出してはならない)
		boost::asio::serial_port port{io};

		// strand。上記の理由により同時に呼び出せないasync_read, async_writeが同時に実行されないことを保証するためのもの
		boost::asio::io_context::strand write_strand{io};
		boost::asio::io_context::strand read_strand{io};

		// エラーを表示するファンクタ
		ErrorReporter error_reporter;
		InfoReporter info_reporter;


	public:
		CobsSerial(const std::string_view port_filepath, const unsigned int baud_rate, ErrorReporter&& error_reporter = ErrorReporter(), InfoReporter&& info_reporter = InfoReporter()) noexcept(false):
			error_reporter{std::move(error_reporter)},
			info_reporter{std::move(info_reporter)}
		{
			boost::system::error_code ec;

			// openする
			port.open(std::string(port_filepath), ec);
			if(ec)
			{
				error_reporter((__FILE__ ": line " Strnize2(__LINE__) ": cannot boost::asio::serial_port::cansel." + ec.message()).c_str());
				goto end;
			}
			
			// baudrateをセット
			port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));

			/// @todo 1秒くらい待つ(下の処理を1秒後に走らせる) <- いるかぁ？
			 boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
			 t.wait();

			// 処理を別スレッドで走らせる
			boost::asio::post(thread_pool,
				[this]
				{
					try
					{
						io.run();
					}
					catch(...)
					{
						this->error_reporter(__FILE__ ": line " Strnize2(__LINE__) ": error: boost::asio::io_context::run() throws an exception.");
					}
				}
			);

			info_reporter("CobsSerial initialize completed.");

			end:;
		}

		~CobsSerial()
		{
			// io.run()を止めるよう要求
			io.stop();

			// io.run()が止まるまで待機
			thread_pool.join();

			// portを閉じる
			boost::system::error_code ec;
			port.cancel(ec);
			if(ec) error_reporter((__FILE__ ": line " Strnize2(__LINE__) ": error: cannot boost::asio::serial_port::cansel.: " + ec.message()).c_str());
			port.close(ec);
			if(ec) error_reporter((__FILE__ ": line " Strnize2(__LINE__) ": error: cannot boost::asio::serial_port::close.: " + ec.message()).c_str());

			info_reporter("CobsSerial close completed.");
		}

		// 逆COBS変換されたフレームを受け取るハンドラを受け取る
		template<class ReadHandler>
		requires requires(ReadHandler read_handler, std::vector<u8> frame)
		{
			{read_handler(frame)} noexcept;
		}
		void async_read(ReadHandler&& read_handler) noexcept
		{
			async_read_inner(std::move(read_handler), std::make_unique<std::vector<u8>>());
		}

		void async_write(const std::vector<u8>& frame) noexcept
		{
			async_write_inner(std::make_unique<std::vector<u8>>(cobs(frame)));
		}

	private:
		// 例外発生時に再帰するのでinnerを用意
		template<class ReadHandler>
		void async_read_inner(ReadHandler&& read_handler, std::unique_ptr<std::vector<u8>>&& cobsed_frame) noexcept
		{
			boost::asio::post(read_strand, [this, read_handler = std::move(read_handler), cobsed_frame = std::move(cobsed_frame)]() mutable
			{
				// 引数の評価順序が未規定であることに注意
				// ↑先にmoveされるかもって話か。なるほど過去の俺賢い
				//  ↑いや、関数オブジェクトをムーブすればいいだろ
				const auto cobsed_frame_p = cobsed_frame.get();
				auto handler = [this, read_handler = std::move(read_handler), cobsed_frame = std::move(cobsed_frame)](const boost::system::error_code& ec, const size_t) mutable  // ここで寿命が移る
					{
						if(ec)
						{
							error_reporter((__FILE__ ": line " Strnize2(__LINE__) ": error: cannot boost::asio::async_read_until.:" + ec.message()).c_str());
							// this->async_read_inner(std::move(read_handler), std::move(cobsed_frame));  // もう一度読む
						}
						else
						{
							read_handler(decobs(*cobsed_frame));  // 0終端してあるのでヨシ
						}
					};
				boost::asio::async_read_until(port, boost::asio::dynamic_buffer(*cobsed_frame_p), '\0',
					
				);
			});
		}

		// 例外発生時に再帰するのでinnerを用意
		void async_write_inner(std::unique_ptr<std::vector<u8>>&& cobsed_frame) noexcept
		{
			boost::asio::post(write_strand, [this, cobsed_frame = std::move(cobsed_frame)]() mutable
			{
				// 読み出し用バッファの寿命をちゃんと考える
				// 引数の評価順序が未規定であることに注意
				const auto cobsed_frame_p = cobsed_frame.get();
				boost::asio::async_write(port, boost::asio::buffer(*cobsed_frame_p),
					[this, cobsed_frame = std::move(cobsed_frame)](const boost::system::error_code& ec, const size_t) mutable
					{
						if(ec)
						{
							error_reporter((__FILE__ ": line " Strnize2(__LINE__) ": error: cannot boost::asio::async_write.:" + ec.message()).c_str());
							// this->async_write_inner(std::move(cobsed_frame));  // もう一度書く
						}
					}
				);
			}
			);
		}
	};
}

#undef Strnize1
#undef Strnize2