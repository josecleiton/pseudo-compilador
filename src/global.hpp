/*
 * =====================================================================================
 *
 *       Filename:  global.hpp
 *
 *    Description: Globals
 *
 *        Version:  1.0
 *        Created:  03/09/2020 05:11:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once
#include <fstream>
#include <string>
#if __GNUC__ > 7
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

/* Variável global que guarda o caminho para o arquivo de entrada */
extern fs::path G_filepath;
