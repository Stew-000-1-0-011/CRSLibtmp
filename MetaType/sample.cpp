#include "type.hpp"

namespace Hoge
{
	template<size_t n>
	using str = CRSLib::StringLiteral<char, n>;

	using namespace CRSLib::MetaType::Declaration;

	constexpr auto pid_t = record
	<
		item<id<str{"p"}>, f64>,
		item<id<str{"i"}>, f64>,
		item<id<str{"d"}>, f64>
	>;


	constexpr auto motor_t = record
	<
		item<id<str{"id"}>, i32>,
		item<id<str{"pid"}>, pid_t>
	>;

	constexpr auto config_t =
	record
	<
		item<id<str{"limit"}>, f64>,
		item<id<str{"motors"}>, array<motor_t, 4>>
	>;
}

int main()
{
	[[maybe_unused]] CRSLib::MetaType::Value<decltype(Hoge::config_t)> v{};
}
