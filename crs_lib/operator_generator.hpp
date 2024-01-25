#pragma once

#include "string_literal.hpp"

namespace crs_lib::operator_generator::impl {
	template<class Arg_, class Ret_>
	using UnaryOpFunc = auto (Arg_) -> Ret_;

	template<string_literal::StringLiteral op_symbol, auto * op_>
	struct UnaryOp;

	template<auto * op_>
	struct UnaryOpWrapper;

	template<class Arg_, class Ret_, UnaryOpFunc<Arg_, Ret_> * op_>
	struct UnaryOpWrapper<op_> final {
		constexpr auto operator*(Arg_ arg) -> Ret_ {
			return op_(static_cast<Arg_&&>(arg));
		}
	};

	template<string_literal::StringLiteral op_symbol, auto op_>
	struct AutoUnaryOp;

	template<auto op_>
	struct AutoUnaryOpWrapper final {
		constexpr auto operator*(auto&& arg) -> decltype(auto) {
			return op_(static_cast<decltype(arg)&&>(arg));
		}
	};

	#define cRsLib_OPERATOR_GENERATOR_UNARY_OP(op_symbol) \
	template<class Arg_, class Ret_, UnaryOpFunc<Arg_, Ret_> * op_>\
	struct UnaryOp<#op_symbol, op_> {\
		static constexpr auto op = op_;\
\
		constexpr auto operator op_symbol() const noexcept {\
			return UnaryOpWrapper<op_>{};\
		}\
	};\
\
	template<auto op_>\
	struct AutoUnaryOp<#op_symbol, op_> {\
		static constexpr auto op = op_;\
\
		constexpr auto operator op_symbol() const noexcept {\
			return AutoUnaryOpWrapper<op_>{};\
		}\
	};

	cRsLib_OPERATOR_GENERATOR_UNARY_OP(++)
	cRsLib_OPERATOR_GENERATOR_UNARY_OP(--)
	cRsLib_OPERATOR_GENERATOR_UNARY_OP(~)
	cRsLib_OPERATOR_GENERATOR_UNARY_OP(!)

	#undef cRsLib_OPERATOR_GENERATOR_UNARY_OP



	template<class L_, class R_, class Ret_>
	using BinaryOpFunc = auto (L_, R_) -> Ret_;

	template<string_literal::StringLiteral op_symbol, auto * op_>
	struct BinaryLeftOp;

	template<string_literal::StringLiteral op_symbol, auto * op_>
	struct PartialAppliedBinaryLeftOp;

	template<string_literal::StringLiteral op_symbol, auto op_>
	struct AutoBinaryLeftOp;

	template<string_literal::StringLiteral op_symbol, class L_, auto op_>
	struct AutoPartialAppliedBinaryLeftOp;

	template<string_literal::StringLiteral op_symbol, auto * op_>
	struct PartialAppliedBinaryLeftOpHasUnary;

	template<string_literal::StringLiteral op_symbol, class L_, auto op_>
	struct AutoPartialAppliedBinaryLeftOpHasUnary;

	#define cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(op_symbol)\
	template<class L_, class Arg_, class Ret_, BinaryOpFunc<L_, Arg_, Ret_> * op_>\
	struct PartialAppliedBinaryLeftOpHasUnary<#op_symbol, op_> {\
		L_ l;\
\
		constexpr auto operator op_symbol(Arg_ r) && -> Ret_ {\
			return op_(static_cast<L_&&>(l), static_cast<Arg_&&>(r));\
		}\
	};\
\
	template<class L_, auto op_>\
	struct AutoPartialAppliedBinaryLeftOpHasUnary<#op_symbol, L_, op_> {\
		L_&& l;\
\
		constexpr auto operator op_symbol(auto&& r) && -> decltype(auto) {\
			return op_(static_cast<L_&&>(l), static_cast<decltype(r)&&>(r));\
		}\
	};\
\
	template<class L_, class R_, class Ret_, BinaryOpFunc<L_, R_, Ret_> * op_>\
	struct PartialAppliedBinaryLeftOp<#op_symbol, op_> {\
		L_ l;\
\
		constexpr auto operator op_symbol(R_ r) && -> Ret_ {\
			return op_(static_cast<L_&&>(l), static_cast<R_&&>(r));\
		}\
\
		template<class Arg_, class Ret2_, UnaryOpFunc<Arg_, Ret2_> * unary_op_>\
		constexpr auto operator op_symbol(const UnaryOpWrapper<unary_op_>&) && -> PartialAppliedBinaryLeftOpHasUnary <\
			#op_symbol, +[](L_ l, Arg_ r){\
				return op_(static_cast<L_&&>(l), unary_op_(static_cast<Arg_&&>(r)));\
			}\
		> {\
			return {static_cast<L_&&>(l)};\
		}\
\
		template<auto unary_op_>\
		constexpr auto operator op_symbol(const AutoUnaryOpWrapper<unary_op_>&) && -> AutoPartialAppliedBinaryLeftOpHasUnary <\
			#op_symbol, L_\
			, [](L_&& l, auto&& r){\
				return op_(static_cast<L_&&>(l), unary_op_(static_cast<decltype(r)&&>(r)));\
			}\
		> {\
			return {static_cast<L_&&>(l)};\
		}\
	};\
\
	template<class L_, class R_, class Ret_, BinaryOpFunc<L_, R_, Ret_> * op_>\
	struct BinaryLeftOp<#op_symbol, op_> {\
		friend constexpr auto operator op_symbol(L_ l, const BinaryLeftOp&) -> PartialAppliedBinaryLeftOp<#op_symbol, op_> {\
			return PartialAppliedBinaryLeftOp<#op_symbol, op_>{static_cast<L_&&>(l)};\
		}\
	};\
\
	template<class L_, auto op_>\
	struct AutoPartialAppliedBinaryLeftOp<#op_symbol, L_, op_> {\
		L_&& l;\
\
		constexpr auto operator op_symbol(auto&& r) && -> decltype(auto) {\
			return op_(static_cast<L_&&>(l), static_cast<decltype(r)&&>(r));\
		}\
\
		template<class Arg_, class Ret2_, UnaryOpFunc<Arg_, Ret2_> * unary_op_>\
		constexpr auto operator op_symbol(const UnaryOpWrapper<unary_op_>&) && -> PartialAppliedBinaryLeftOpHasUnary <\
			#op_symbol, +[](L_ l, Arg_ r){\
				return op_(static_cast<L_&&>(l), unary_op_(static_cast<Arg_&&>(r)));\
			}\
		> {\
			return {static_cast<L_&&>(l)};\
		}\
\
		template<auto unary_op_>\
		constexpr auto operator op_symbol(const AutoUnaryOpWrapper<unary_op_>&) && -> AutoPartialAppliedBinaryLeftOpHasUnary <\
			#op_symbol, L_\
			, [](L_&& l, auto&& r){\
				return op_(static_cast<L_&&>(l), unary_op_(static_cast<decltype(r)&&>(r)));\
			}\
		> {\
			return {static_cast<L_&&>(l)};\
		}\
	};\
\
	template<auto op_>\
	struct AutoBinaryLeftOp<#op_symbol, op_> {\
		friend constexpr auto operator op_symbol(auto&& l, const AutoBinaryLeftOp&) -> AutoPartialAppliedBinaryLeftOp<#op_symbol, decltype(l), op_> {\
			return AutoPartialAppliedBinaryLeftOp<#op_symbol, decltype(l), op_>{static_cast<decltype(l)&&>(l)};\
		}\
	};

	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(*)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(/)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(%)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(+)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(-)

	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(<<)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(>>)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(<)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(<=)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(>)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(>=)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(==)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(!=)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(&)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(^)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(|)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(&&)
	cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP(||)

	#undef cRsLib_OPERATOR_GENERATOR_BINARY_LEFT_OP
}

namespace crs_lib::operator_generator {
	using impl::AutoUnaryOp;
	using impl::AutoBinaryLeftOp;
	using impl::AutoPartialAppliedBinaryLeftOp;
}
