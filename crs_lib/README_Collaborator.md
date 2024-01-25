# はじめに
**めっちゃ長いです！ごめん！！**  
リファレンス的なものなので、ざらっと斜め読みしてくれれば十分です。 
仲間内で意見が割れたときにここを当たり、必要に応じて変更・改善してください。

# インデント
- C/C++ではTabを使う
- 改行文字の前には空白を入れない

## 括弧内で改行するとき/しないとき
```
# 各非終端記号は空白や改行を含まない
Snippet := Others Bracket-L Contents Bracket-R ?Separator Others
```
とすると、
```
Indent(n) Others ' ' Bracket-L
Indent(n + 1) Contents
Indent(n) Bracket-R ?Separator
Indent(n) Others
```
あるいは
```
Indent(n) Others Bracket-L Contents Bracket-R Others
```
とすること。
つまり、改行するなら左括弧前に空白を一つ入れ、括弧内はインデントを一つ上げ、 
そうでないならどこにも空白を入れない。

ただし、例外として型の宣言においては型名と括弧の間に空白を入れる。
継承を書くときに空白を入れないとどうにもしっくりこないので。

## 中置演算子と改行
中置演算子近辺で改行したくなったら、中置演算子前で改行すること。

## 区切り文字と改行
```
Sequence := *(Element Separator)
```
ならSeparatorは改行前に入れ、
```
Sequence := ?Element *(Separator Element)
```
ならSeparatorは改行+インデント後に入れること。

## if文と改行
### compound statementでないsubstatementを持つif文
す・る・な。改行したいならcompound statementにしろ。
### dangling else
できればcompound statementにしてほしい。
そうでなくても、if文の最初のsub statementがelse句を含むif文となる場合、そのelse句は絶対に改行しないこと。

## 例
```cpp
struct A final {
	int x;
	int y;
};

int main() {
	auto a = A {
		.x = 42
		, .y = -41
	};

	a.x = 0b0100'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000
		/ 0b0100'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000;
	
	// // NG!!!
	// if(false)
	// std::cout << "Oops!" << std::endl;

	// // NG!!!
	// if(false)
	// 	std::cout << "Oops!" << std::endl;

	// // NG!!!
	// if(false) if(false) std::cout << "Oops!" << std::endl;
	// else std::cout << "Oops!" << std::endl;

	// OK
	if(false) std::cout << "Good!" << std::endl;
	
	if(false) {
		std::cout << "Good!" << std::endl;
	}
	
	if(false) if(false) std::cout << "Good!" << std::endl; else std::cout << "Good!" << std::endl;
	
	if(false) {
		if(false) {
			std::cout << "Good!" << std::endl;
		} else {
			std::cout << "Good!" << std::endl;
		}
	}
}
```

# 命名規則
- C++の識別子や名前に関する規則に**必ず**したがってください。(参考:[C++20ドラフト lex.nameの3](https://timsong-cpp.github.io/cppwp/n4868/lex.name#3))
- よほど特別な理由が無ければ(e.g. 自動生成ツールに従わざるを得ないなど)、以下の規則に従ってください

## 識別子のletter case
もちろん、kebab-caseは使えません。
- 名前空間: `lower_snake_case`
- 型、型パラメータ: `PascalCase` の末尾に`_`
- コンセプト: `lower_snake_case`
- マクロ: 基本は`UPPER_SMALL_CASE`, ただし先頭に`cRsLib_`を付ける  
  **アンダースコア`_`を2回続けないこと！これをインクルードガードでやってるのは誤りです！！**

### なぜこうしたのか
- 名前空間と型、型とコンセプト、型と関数や値などを区別したい
- 型パラメータをT_を再度宣言することが多いので区別したい e.g. `template<class ElementType_> struct Holder { using ElementType = ElementType_; /* ... */ };`
- マクロは他の人たちとケースを極力合わせたい(他の人がマクロ以外に使っているものと被ると容易に死ぬ [例: Windowsのmin, maxマクロ問題](https://yohhoy.hatenadiary.jp/entry/20120115/p1))
  そして他のマクロと被るっても死ぬので、`cRsLib_`を付けることで回避
- 多くの識別子が指し示すのはだいたいimmutableであろうので、特に定数に対する追加の規則は設けない

## 品詞について
自由。ただ、以下が多いと思う：
- 関数名は述部っぽい
- 関数っぽくないものは述部っぽくない
  - 基本名詞っぽいが稀に副詞や形容詞っぽかったりするかも

## 略語について
- 略語は自明な場合にのみ使うこと
  - 判断基準は「そのコードが動くものを解説するとして、聴衆への説明にその略語を使うか/その略語を説明するか」
- 略語ひとまとまりをsnake_caseで書くならばアンダースコアを入れず、PascalCaseで書くならば先頭のみを大文字にすること
  - e.g. CAN(Controller Area Network) -> `can` or `Can`
  - これは他の命名規則におけるマクロに使われる`UPPER_SNAKE_CASE`と被る事態を極力避けるため

## ハンガリアン記法について
できるなら使わないが、絶対使うなとは言わない。

使う前に、以下をチェックすること：
- それは型から得られる情報ではないか？
  型は大抵のエディタで簡単に確かめられるので、そちらを頼りにすべき
- ハンガリアン記法が2, 3個繋がってないか？
  何か設計悪くない...？それ、今の型をラップした新たな型を作るべきだったりしない？

## 名前の長さについて
**必要なら勇気をもって長く書いて！**必要な情報が本当に長いなら、それを略されるとさらに辛い。
ただ...
- 型にその情報は入ってないか？/その情報は型が持つべきでは？
- 名前空間にその情報は入ってないか？/名前空間を分けるべきでは？

## ローカルでない名前について
- `crs_lib::ディレクトリ1::ディレクトリ2::...::ファイル名`という名前空間に入れよう
  - 全て`crs_lib`名前空間に入れること
  - ある名前空間`foo`の中でのみ使うものは`foo::impl`に入れる
    - 外に出したい名前を宣言するときに`impl::`と書くのが面倒なら、`foo::impl`内で宣言し、後で`using`で外に出すのがおすすめ
      e.g.
      ```cpp
      namespace crs_lib::foo::impl {
          struct LocallyUsed final {};
          struct Exported final {
              LocallyUsed bar;
          };
      }
      namespace crs_lib::foo {
          using impl::Exported;
      }
      ```
- 演算子オーバーロードはhidden friendsを使おう

# ヘッダーファイルについて
moduleを使えそうになったら、なるはやで移転させてほしいです。
- メンバ関数はヘッダーへ
  - コンパイル時間をどうしても削減したい、とかでもない限りはヘッダオンリーで書こう
- `pragma once`推奨
- inline指定子を忘れないように

# マクロについて
**使・う・な**。  
`constexpr`/`consteval`を使おう。  
...トークンを操作したいならまあ、使ってもいい。ただし**必ず`undef`すること**。

# クラスについて
## コンストラクタについて
structのコンストラクタは極力自明にしよう / 
複雑な初期化処理はそれ用のstaticメンバ関数を用意しよう


## 継承について
**本当に継承じゃなきゃダメか？**
1. liskov substitution principleを満たしているか？
  そうでなく、hackなことしてるとかでもないなら、継承はやめよう。それは包含でいい。

ポリモーフィズムのためであったとして、
1. その抽象化は的を得ているか？
2. それは継承じゃなきゃダメか？
  - 大抵はコンセプトでなんとかなるぞ？
  - あるいはstd::variantがあるぞ？
3. それは以下の難点を呑めるほどの価値があるか？
  - destructorを`virtual`にしないといけなくなるかも。そこら辺面倒になるぞ？
    - downcastしたくならないか？そうならば、健全性は保証されているか？
  - オーバーロードを汚染しないか？
  - メモリ配置を気にする必要がないか？
継承させるつもりがないならfinalを付けよう

## "しないこと"の保証について
- 例外を吐かないなら`noexcept`を付けよう / `std::bad_alloc`以外を吐き得るなら`noexcept(false)`を付けよう
- 値の更新の必要がないなら`const`を付けよう
- 継承させるつもりがないなら`final`を付けよう
- `constexpr`にできるなら`constexpr`にしよう
- `consteval`でいいなら`consteval`にしよう