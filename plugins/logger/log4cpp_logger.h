/**
 *       @file  log4cpp.h
 *      @biief  A Logger plugin based on Log4Cpp library
 *
 * This defines a Log4CPP based Logger plugin.
 *
 *     @author  Patrick Bellasi (derkling), derkling@google.com
 *
 *   @internal
 *     Created  01/11/2011
 *    Revision  $Id: doxygen.templates,v 1.3 2010/07/06 09:20:12 mehner Exp $
 *    Compiler  gcc/g++
 *     Company  Politecnico di Milano
 *   Copyright  Copyright (c) 2011, Patrick Bellasi
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */

#ifndef BBQUE_LOG4CPP_LOGGER_H_
#define BBQUE_LOG4CPP_LOGGER_H_

#include "bbque/plugins/logger.h"
#include "bbque/plugins/plugin.h"

#include <cstdint>
#include <log4cpp/Category.hh>

// These are the parameters received by the PluginManager on create calls
struct PF_ObjectParams;

namespace bbque { namespace plugins {

/**
 * @class Log4CppLogger
 * @brief The basic class for each Barbeque component
 */
class Log4CppLogger : public LoggerIF {

public:

//----- static plugin interface

	/**
	 * @brief   
	 * @param   
	 * @return  
	 */
	static void * Create(PF_ObjectParams * params);

	/**
	 * @brief   
	 * @param   
	 * @return  
	 */
	static int32_t Destroy(void * logger);

	/**
	 * @brief   
	 * @param   
	 * @return  
	 */
	~Log4CppLogger();

//----- Logger module interface

	/**
	 * \brief Send a log message with the priority DEBUG
	 * \param message the message to log
	 */
	void Debug(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority INFO
	 * \param message the message to log
	 */
	void Info(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority NOTICE
	 * \param message the message to log
	 */
	void Notice(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority WARN
	 * \param message the message to log
	 */
	void Warn(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority ERROR
	 * \param message the message to log
	 */
	void Error(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority CRIT
	 * \param message the message to log
	 */
	void Crit(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority ALERT
	 * \param message the message to log
	 */
	void Alert(const char *fmt, ...);

	/**
	 * \brief Send a log message with the priority FATAL
	 * \param message the message to log
	 */
	void Fatal(const char *fmt, ...);


private:

	/**
	 * Set true to use colors for logging
	 */
	bool use_colors;

	/**
	 * @brief The logger reference
	 * Use this logger reference, related to the 'log' category, to log your messages
	 */
	log4cpp::Category & logger;

	/**
	 * @brief Build a new Barbeque component
	 * Each Barbeque component is associated to a logger category whose
	 * name is prefixed by "bbque."
	 * @param logName the log category, this name is (forcely) prepended by the
	 * 	class namespace "bbque."
	 */
	Log4CppLogger(std::string const & name = "undef");

};

} // namespace plugins

} // namespace bbque

#endif // BBQUE_LOG4CPP_LOGGER_H_

