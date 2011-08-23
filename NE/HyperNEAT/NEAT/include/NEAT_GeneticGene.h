#ifndef __GENETICGENE_H__
#define __GENETICGENE_H__

#include "NEAT_Globals.h"

namespace NEAT
{

    /**
     * The Genetic Gene class is responsible for holding a single gene in a GA string
     */
    class NEAT_DLL_EXPORT GeneticGene
    {
    protected:
        int ID;

        bool enabled;

        int age;
    public:

        /**
         * Constructor: Creates and enables a new Genetic Gene.
         */
        GeneticGene();

        /**
         * Constructor: Creates a GeneticGene from a serialized XML format
         */
        GeneticGene(TiXmlElement *elementPtr);

        GeneticGene(istream &istr);

        virtual ~GeneticGene();

        virtual bool operator==(const GeneticGene &other) const;

        /**
         * mutate: Gets called when a gene undergoes mutation
         */
        virtual void mutate() = 0;

        inline const int &getID() const
        {
            return ID;
        }

        inline void setID(int _ID)
        {
            ID = _ID;
        }

        /**
         * disable: disables this gene, removing its functionality
         */
        inline void disable()
        {
            enabled=false;
        }

        /**
         * toggleEnabled: flips the enabled status of this gene
         */
        inline void toggleEnabled()
        {
            enabled=!enabled;
        }

        inline void setEnabled(bool enabledVal)
        {
            enabled = enabledVal;
        }

        /**
         * isEnabled: returns wheter or not this gene is enabled
         */
        inline const bool isEnabled() const
        {
            return enabled;
        }

        /**
         * dump: serializes this gene to an XML format
         */
        virtual void dump(TiXmlElement *XMLnode);

        virtual void dump(ostream &ostr);

        inline void incrementAge()
        {
            age++;
        }

        inline const int &getAge()
        {
            return age;
        }

        inline void setAge(int newAge)
        {
            age = newAge;
        }
    };

}

#endif
