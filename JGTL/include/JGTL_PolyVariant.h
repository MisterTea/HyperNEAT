#ifndef POLY_VARIANT_H_INCLUDED
#define POLY_VARIANT_H_INCLUDED

#include <iostream>
#include "JGTL_LocatedException.h"

#include "JGTL_Variant.h"

namespace JGTL
{

	template<
		class BaseClass,
		class Class1,
		class Class2=NullVariantClass2,
		class Class3=NullVariantClass3,
		class Class4=NullVariantClass4,
		class Class5=NullVariantClass5,
		class Class6=NullVariantClass6,
		class Class7=NullVariantClass7,
		class Class8=NullVariantClass8,
		class Class9=NullVariantClass9,
		class Class10=NullVariantClass10
	>
	class PolyVariant : public Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>
	{
	protected:
		BaseClass* base;

		using Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>::data;
		using Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>::typeOfData;

	public:
		using Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>::clearValue;

		PolyVariant()
			:
		Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>()
		{
		}

		PolyVariant(const PolyVariant<BaseClass,Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other)
			:
				Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>(other),
				base( (BaseClass*)data )
		{
		}

		BaseClass* operator->() const
		{
			return base;
		}

		BaseClass* getBaseValue() const
		{
			return base;
		}

#define JGTL_POLY_VARIANT_CASE_MACRO_ONE(NUMBER) \
            else if(typeid(ValueToSet) == typeid(Class ## NUMBER)) \
			{\
				typeOfData = NUMBER;\
			}\

		template<class ValueToSet>
		void setValue(const ValueToSet &newValue)
		{
			clearValue();
			new(data) ValueToSet(newValue);
			base = (BaseClass*)((ValueToSet*)data);
            if(false)
            {
            }
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(1)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(2)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(3)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(4)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(5)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(6)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(7)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(8)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(9)
            JGTL_POLY_VARIANT_CASE_MACRO_ONE(10)
			else
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to set the value of a variant with a type that is not supported");
			}
		}
	};

}

#endif // POLY_VARIANT_H_INCLUDED
