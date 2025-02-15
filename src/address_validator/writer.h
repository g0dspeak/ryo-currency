// Copyright (c) 2020, pasta Currency Project
// Portions copyright (c) 2014-2018, The Monero Project
//
// Portions of this file are available under BSD-3 license. Please see ORIGINAL-LICENSE for details
// All rights reserved.
//
// Authors and copyright holders give permission for following:
//
// 1. Redistribution and use in source and binary forms WITHOUT modification.
//
// 2. Modification of the source form for your own personal use.
//
// As long as the following conditions are met:
//
// 3. You must not distribute modified copies of the work to third parties. This includes
//    posting the work online, or hosting copies of the modified work for download.
//
// 4. Any derivative version of this work is also covered by this license, including point 8.
//
// 5. Neither the name of the copyright holders nor the names of the authors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// 6. You agree that this licence is governed by and shall be construed in accordance
//    with the laws of England and Wales.
//
// 7. You agree to submit all disputes arising out of or in connection with this licence
//    to the exclusive jurisdiction of the Courts of England and Wales.
//
// Authors and copyright holders agree that:
//
// 8. This licence expires and the work covered by it is released into the
//    public domain on 1st of February 2021
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

/*!
 * \namespace cryptonote
 * \brief Holds cryptonote related classes and helpers.
 */
namespace cryptonote
{
/*! print data into a file or on stdout
   */
class writer
{
  public:
	/*! constructor
     *
     * @param filename name of the file, if empty output is written to stdout
     * @param remove_spaces if true print data without spaces
     */
	writer(const std::string &filename, const bool remove_spaces = false) : m_filename(filename), m_remove_spaces(remove_spaces)
	{
		if(!m_filename.empty())
		{
			m_out.open(m_filename);
			m_has_filestream = m_out.is_open();
		}
	};

	~writer()
	{
		if(m_has_filestream)
			m_out.close();
	}

	/*! print data */
	template <typename T>
	writer &operator<<(const T &value)
	{
		print(value);
		return *this;
	}

	/*! print char arrays */
	writer &operator<<(const char *value);

	/*! print single characters */
	writer &operator<<(const char value);

  private:
	/*! print data to file or stdout */
	template <typename T>
	void print(const T &value)
	{
		if(!m_has_filestream)
			std::cout << value;
		else
			m_out << value;
	}

	std::string m_filename;
	bool m_remove_spaces = false;
	std::ofstream m_out;
	bool m_has_filestream = false;
};
}
