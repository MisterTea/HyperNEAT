#ifndef VARIANT_H_INCLUDED
#define VARIANT_H_INCLUDED

#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <cstring>
#include "JGTL_LocatedException.h"

namespace JGTL
{

  template <class Type,bool condition, Type Then, Type Else>
    struct TYPEIF
    {
      static const Type RESULT=Then;
    };

  template <class Type,Type Then, Type Else>
    struct TYPEIF<Type,false, Then, Else>
    {
      static const Type RESULT=Else;
    };

  template <
    class One,
	  class Two = One,
	  class Three = One,
	  class Four = One,
	  class Five = One,
	  class Six = One,
	  class Seven = One,
	  class Eight = One,
	  class Nine = One,
	  class Ten = One
	    >
	    struct STATIC_MAX_SIZE
	    {
	      const static int RESULT =
		TYPEIF<
		int,
	      (sizeof(Two) > sizeof(Three)),
	      STATIC_MAX_SIZE<One,One,Two,Four,Five,Six,Seven,Eight,Nine,Ten>::RESULT,
	      STATIC_MAX_SIZE<One,One,Three,Four,Five,Six,Seven,Eight,Nine,Ten>::RESULT
		>::RESULT;
	    };
  template <
    class One,
	  class Two,
	  class Three,
	  class Four,
	  class Five,
	  class Six,
	  class Seven,
	  class Eight,
	  class Nine
	    >
	    struct STATIC_MAX_SIZE<One,One,Two,Three,Four,Five,Six,Seven,Eight,Nine>
	    {
	      const static int RESULT =
		TYPEIF<
		int,
	      (sizeof(Two) > sizeof(Three)),
	      STATIC_MAX_SIZE<One,One,One,Two,Four,Five,Six,Seven,Eight,Nine>::RESULT,
	      STATIC_MAX_SIZE<One,One,One,Three,Four,Five,Six,Seven,Eight,Nine>::RESULT
		>::RESULT;
	    };
  template <
    class One,
	  class Two,
	  class Three,
	  class Four,
	  class Five,
	  class Six,
	  class Seven,
	  class Eight
	    >
	    struct STATIC_MAX_SIZE<One,One,One,Two,Three,Four,Five,Six,Seven,Eight>
	    {
	      const static int RESULT =
		TYPEIF<
		int,
	      (sizeof(Two) > sizeof(Three)),
	      STATIC_MAX_SIZE<One,One,One,One,Two,Four,Five,Six,Seven,Eight>::RESULT,
	      STATIC_MAX_SIZE<One,One,One,One,Three,Four,Five,Six,Seven,Eight>::RESULT
		>::RESULT;
	    };
  template <
    class One,
	  class Two,
	  class Three,
	  class Four,
	  class Five,
	  class Six,
	  class Seven
	    >
	    struct STATIC_MAX_SIZE<One,One,One,One,Two,Three,Four,Five,Six,Seven>
	    {
	      const static int RESULT =
		TYPEIF<
		int,
	      (sizeof(Two) > sizeof(Three)),
	      STATIC_MAX_SIZE<One,One,One,One,One,Two,Four,Five,Six,Seven>::RESULT,
	      STATIC_MAX_SIZE<One,One,One,One,One,Three,Four,Five,Six,Seven>::RESULT
		>::RESULT;
	    };
  template <
    class One,
	  class Two,
	  class Three,
	  class Four,
	  class Five,
	  class Six
	    >
	    struct STATIC_MAX_SIZE<One,One,One,One,One,Two,Three,Four,Five,Six>
	    {
	      const static int RESULT =
		TYPEIF<
		int,
	      (sizeof(Two) > sizeof(Three)),
	      STATIC_MAX_SIZE<One,One,One,One,One,One,Two,Four,Five,Six>::RESULT,
	      STATIC_MAX_SIZE<One,One,One,One,One,One,Three,Four,Five,Six>::RESULT
		>::RESULT;
	    };
  template <class One, class Two, class Three, class Four, class Five>
    struct STATIC_MAX_SIZE<One,One,One,One,One,One,Two,Three,Four,Five>
    {
      const static int RESULT =
	TYPEIF<
	int,
	(sizeof(Two) > sizeof(Three)),
	STATIC_MAX_SIZE<One,One,One,One,One,One,One,Two,Four,Five>::RESULT,
	STATIC_MAX_SIZE<One,One,One,One,One,One,One,Three,Four,Five>::RESULT
	  >::RESULT;
    };
  template <class One, class Two, class Three, class Four>
    struct STATIC_MAX_SIZE<One,One,One,One,One,One,One,Two,Three,Four>
    {
      const static int RESULT =
	TYPEIF<
	int,
	(sizeof(Two) > sizeof(Three)),
	STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,Two,Four>::RESULT,
	STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,Three,Four>::RESULT
	  >::RESULT;
    };
  template <class One, class Two, class Three>
    struct STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,Two,Three>
    {
      const static int RESULT =
	TYPEIF<
	int,
	(sizeof(Two) > sizeof(Three)),
	STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,One,Two>::RESULT,
	STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,One,Three>::RESULT
	  >::RESULT;
    };
  template <class One, class Two>
    struct STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,One,Two>
    {
      const static int RESULT =
	TYPEIF<
	int,
	(sizeof(One) > sizeof(Two)),
	sizeof(One),
	sizeof(Two)
	  >::RESULT;
    };

#define NULL_VARIANT_CLASS(NUMBER) \
    class NullVariantClass ## NUMBER \
  { \
  public: \
      bool operator==(const NullVariantClass ## NUMBER &other) const {return true;} \
  };

    NULL_VARIANT_CLASS(2)
    NULL_VARIANT_CLASS(3)
    NULL_VARIANT_CLASS(4)
    NULL_VARIANT_CLASS(5)
    NULL_VARIANT_CLASS(6)
    NULL_VARIANT_CLASS(7)
    NULL_VARIANT_CLASS(8)
    NULL_VARIANT_CLASS(9)
    NULL_VARIANT_CLASS(10)
  
  template<
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
	    class Variant
	    {
	      protected:
		char data[
		  STATIC_MAX_SIZE<
		  Class1,
		  Class2,
		  Class3,
		  Class4,
		  Class5,
		  Class6,
		  Class7,
		  Class8,
		  Class9,
		  Class10
		    >::RESULT
		    ];
		unsigned char typeOfData;
	      public:
		Variant()
		  :
		    typeOfData(0)
	      {}

#define JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(NUMBER) \
		explicit Variant(const Class ## NUMBER &value)\
		{\
		  setValue<Class ## NUMBER>(value);\
		}\

		JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(1)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(2)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(3)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(4)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(5)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(6)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(7)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(8)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(9)
		  JGTL_VARIANT_CASE_CONSTRUCTOR_MACRO(10)

#define JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(NUMBER) \
		  const Variant &operator=(const Class ## NUMBER & value)\
		  {\
			clearValue();\
		    setValue<Class ## NUMBER>(value);\
		    return *this;\
		  }\

		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(1)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(2)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(3)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(4)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(5)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(6)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(7)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(8)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(9)
		  JGTL_VARIANT_CASE_ASSIGNMENT_MACRO(10)

		  Variant(const Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other)
		  :
		    typeOfData(0)
	      {

#define JGTL_VARIANT_CASE_MACRO_ONE(NUMBER) \
		case NUMBER:\
			    {\
			      setValue(other.getValue<Class ## NUMBER>()); \
			      break;\
			    }

		switch(other.typeOfData)
		{
		  case 0:
		    {
		      break;
		    }
		    JGTL_VARIANT_CASE_MACRO_ONE(1)
		      JGTL_VARIANT_CASE_MACRO_ONE(2)
		      JGTL_VARIANT_CASE_MACRO_ONE(3)
		      JGTL_VARIANT_CASE_MACRO_ONE(4)
		      JGTL_VARIANT_CASE_MACRO_ONE(5)
		      JGTL_VARIANT_CASE_MACRO_ONE(6)
		      JGTL_VARIANT_CASE_MACRO_ONE(7)
		      JGTL_VARIANT_CASE_MACRO_ONE(8)
		      JGTL_VARIANT_CASE_MACRO_ONE(9)
		      JGTL_VARIANT_CASE_MACRO_ONE(10)
		  default:
		      break;
		}
	      }

		const Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &operator=(const Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other)
		{
		  if(this==&other)
		    return *this;

		  switch(other.typeOfData)
		  {
		    case 0:
		      {
			break;
		      }
		      JGTL_VARIANT_CASE_MACRO_ONE(1)
			JGTL_VARIANT_CASE_MACRO_ONE(2)
			JGTL_VARIANT_CASE_MACRO_ONE(3)
			JGTL_VARIANT_CASE_MACRO_ONE(4)
			JGTL_VARIANT_CASE_MACRO_ONE(5)
			JGTL_VARIANT_CASE_MACRO_ONE(6)
			JGTL_VARIANT_CASE_MACRO_ONE(7)
			JGTL_VARIANT_CASE_MACRO_ONE(8)
			JGTL_VARIANT_CASE_MACRO_ONE(9)
			JGTL_VARIANT_CASE_MACRO_ONE(10)
		    default:
			break;
		  }

		  return *this;
		}

		~Variant()
		{
		  clearValue();
		}

#define JGTL_VARIANT_CASE_MACRO_EQ_VALUE(NUMBER) \
    case NUMBER: \
    return getValue<Class ## NUMBER>()==other.getValue<Class ## NUMBER>();

        bool operator==(const Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other) const
        {
            if(typeOfData != other.typeOfData)
            {
                return false;
            }

		  switch(typeOfData)
		  {
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(1)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(2)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(3)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(4)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(5)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(6)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(7)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(8)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(9)
            JGTL_VARIANT_CASE_MACRO_EQ_VALUE(10)
		    default:
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			break;
		  }
        }

#define JGTL_VARIANT_CASE_MACRO_NE_VALUE(NUMBER) \
    case NUMBER: \
    return getValue<Class ## NUMBER>()!=other.getValue<Class ## NUMBER>();

        bool operator!=(const Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other) const
        {
            if(typeOfData != other.typeOfData)
            {
                return true;
            }

		  switch(typeOfData)
		  {
		    case 0:
		      {
			break;
		      }
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(1)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(2)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(3)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(4)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(5)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(6)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(7)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(8)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(9)
            JGTL_VARIANT_CASE_MACRO_NE_VALUE(10)
		    default:
			break;
		  }
        }

        unsigned char getTypeOfData() const
		{
		  return typeOfData;
		}

#define JGTL_VARIANT_CASE_MACRO_TWO(NUMBER) \
		case NUMBER:\
			    {\
			      if(typeid(Class ## NUMBER) == typeid(T)) \
			      return true;\
			      else\
			      return false;\
			    }

		template<class T>
		  bool isOfType()
		  {
		    switch(typeOfData)
		    {
		      case 0:
			return false;
			JGTL_VARIANT_CASE_MACRO_TWO(1)
			  JGTL_VARIANT_CASE_MACRO_TWO(2)
			  JGTL_VARIANT_CASE_MACRO_TWO(3)
			  JGTL_VARIANT_CASE_MACRO_TWO(4)
			  JGTL_VARIANT_CASE_MACRO_TWO(5)
			  JGTL_VARIANT_CASE_MACRO_TWO(6)
			  JGTL_VARIANT_CASE_MACRO_TWO(7)
			  JGTL_VARIANT_CASE_MACRO_TWO(8)
			  JGTL_VARIANT_CASE_MACRO_TWO(9)
			  JGTL_VARIANT_CASE_MACRO_TWO(10)
		      default:
			  return false;
			  break;
		    }
		  }

		template<class ReturnValueClass>
		  ReturnValueClass getValue() const
		  {
		    const ReturnValueClass* ptr = getValuePtr<ReturnValueClass>();

		    if(ptr==NULL)
		    {
		      throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to get the value of a variant which was assigned to a different type");
		    }

		    return *ptr;
		  }

		template<class ReturnValueClass>
		  ReturnValueClass &getValueRef()
		  {
		    ReturnValueClass* ptr = getValuePtr<ReturnValueClass>();

		    if(ptr==NULL)
		    {
		      throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to get the reference of a variant which was assigned to a different type");
		    }

		    return *ptr;
		  }

		template<class ReturnValueClass>
		  const ReturnValueClass &getValueRef() const
		  {
		    const ReturnValueClass* ptr = getValuePtr<ReturnValueClass>();

		    if(ptr==NULL)
		    {
		      throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to get the reference of a variant which was assigned to a different type");
		    }

		    return *ptr;
		  }

#define JGTL_VARIANT_CASE_MACRO_THREE(NUMBER) \
		case NUMBER:\
			    {\
			      if(typeid(Class ## NUMBER) != typeid(ReturnValueClass)) \
			      return NULL;\
			      else\
			      break;\
			    }

		template<class ReturnValueClass>
		  ReturnValueClass* getValuePtr()
		  {
		    if(!typeOfData)
		    {
		      throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
		    }

		    switch(typeOfData)
		    {
		      JGTL_VARIANT_CASE_MACRO_THREE(1)
			JGTL_VARIANT_CASE_MACRO_THREE(2)
			JGTL_VARIANT_CASE_MACRO_THREE(3)
			JGTL_VARIANT_CASE_MACRO_THREE(4)
			JGTL_VARIANT_CASE_MACRO_THREE(5)
			JGTL_VARIANT_CASE_MACRO_THREE(6)
			JGTL_VARIANT_CASE_MACRO_THREE(7)
			JGTL_VARIANT_CASE_MACRO_THREE(8)
			JGTL_VARIANT_CASE_MACRO_THREE(9)
			JGTL_VARIANT_CASE_MACRO_THREE(10)
		      default:
			return NULL;
			break;
		    }

		    return ((ReturnValueClass*)data);
		  }

		template<class ReturnValueClass>
		  const ReturnValueClass* getValuePtr() const
		  {
		    if(!typeOfData)
		    {
		      throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
		    }

		    switch(typeOfData)
		    {
		      JGTL_VARIANT_CASE_MACRO_THREE(1)
			JGTL_VARIANT_CASE_MACRO_THREE(2)
			JGTL_VARIANT_CASE_MACRO_THREE(3)
			JGTL_VARIANT_CASE_MACRO_THREE(4)
			JGTL_VARIANT_CASE_MACRO_THREE(5)
			JGTL_VARIANT_CASE_MACRO_THREE(6)
			JGTL_VARIANT_CASE_MACRO_THREE(7)
			JGTL_VARIANT_CASE_MACRO_THREE(8)
			JGTL_VARIANT_CASE_MACRO_THREE(9)
			JGTL_VARIANT_CASE_MACRO_THREE(10)
		      default:
			return NULL;
			break;
		    }

		    return ((const ReturnValueClass*)data);
		  }

#define JGTL_VARIANT_CASE_MACRO_FOUR(NUMBER) \
		case NUMBER:\
			    ((Class##NUMBER*)data)->~Class##NUMBER();\
		break;

		void clearValue()
		{
		  if (typeOfData)
		  {
		    switch(typeOfData)
		    {
		      JGTL_VARIANT_CASE_MACRO_FOUR(1)
			JGTL_VARIANT_CASE_MACRO_FOUR(2)
			JGTL_VARIANT_CASE_MACRO_FOUR(3)
			JGTL_VARIANT_CASE_MACRO_FOUR(4)
			JGTL_VARIANT_CASE_MACRO_FOUR(5)
			JGTL_VARIANT_CASE_MACRO_FOUR(6)
			JGTL_VARIANT_CASE_MACRO_FOUR(7)
			JGTL_VARIANT_CASE_MACRO_FOUR(8)
			JGTL_VARIANT_CASE_MACRO_FOUR(9)
			JGTL_VARIANT_CASE_MACRO_FOUR(10)
		      default:
			break;
		    }
		    typeOfData=0;
		  }

		  memset(data,0,STATIC_MAX_SIZE<
		      Class1,
		      Class2,
		      Class3,
		      Class4,
		      Class5,
		      Class6,
		      Class7,
		      Class8,
		      Class9,
		      Class10
		      >::RESULT );
		}

#define JGTL_VARIANT_CASE_MACRO_FIVE(NUMBER) \
		else if(typeid(ValueToSet) == typeid(Class ## NUMBER))\
		{\
		  typeOfData = NUMBER;\
		}\

		template<class ValueToSet>
		  void setValue(const ValueToSet &newValue)
		  {
		    clearValue();
		    new(data) ValueToSet(newValue);
		    if(false)
		    {
		    }
		    JGTL_VARIANT_CASE_MACRO_FIVE(1)
		      JGTL_VARIANT_CASE_MACRO_FIVE(2)
		      JGTL_VARIANT_CASE_MACRO_FIVE(3)
		      JGTL_VARIANT_CASE_MACRO_FIVE(4)
		      JGTL_VARIANT_CASE_MACRO_FIVE(5)
		      JGTL_VARIANT_CASE_MACRO_FIVE(6)
		      JGTL_VARIANT_CASE_MACRO_FIVE(7)
		      JGTL_VARIANT_CASE_MACRO_FIVE(8)
		      JGTL_VARIANT_CASE_MACRO_FIVE(9)
		      JGTL_VARIANT_CASE_MACRO_FIVE(10)
		    else
		    {
		      throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to set the value of a variant with a type that is not supported");
		    }
		  }
	    };

}

#endif // VARIANT_H_INCLUDED

