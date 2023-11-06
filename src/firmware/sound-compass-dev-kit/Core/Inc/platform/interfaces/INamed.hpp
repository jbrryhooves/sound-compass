/*
 * INamed.hpp
 *
 *  Created on: Oct 16, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_INAMED_HPP_
#define INC_PLATFORM_INTERFACES_INAMED_HPP_
//-------------------------------------------------------------------
// Module       : INamed.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class INamed
    {
    public:
        /**
         * @brief Set name of object
         *
         * @param name
         */
        virtual void setName(const char *name) = 0;

        /**
         * @brief Get name of object
         *
         * @return const char*
         */
        virtual const char* getName(void) = 0;
    };
}

#endif /* INC_PLATFORM_INTERFACES_INAMED_HPP_ */

