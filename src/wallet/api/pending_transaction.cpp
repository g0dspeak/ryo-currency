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
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "pending_transaction.h"
#include "common_defines.h"
#include "wallet.h"

#include "cryptonote_basic/cryptonote_basic_impl.h"
#include "cryptonote_basic/cryptonote_format_utils.h"

#include <boost/format.hpp>
#include <memory>
#include <sstream>
#include <vector>

using namespace std;

namespace pasta
{

PendingTransaction::~PendingTransaction() {}

PendingTransactionImpl::PendingTransactionImpl(WalletImpl &wallet)
	: m_wallet(wallet)
{
	m_status = Status_Ok;
}

PendingTransactionImpl::~PendingTransactionImpl()
{
}

int PendingTransactionImpl::status() const
{
	return m_status;
}

string PendingTransactionImpl::errorString() const
{
	return m_errorString;
}

std::vector<std::string> PendingTransactionImpl::txid() const
{
	std::vector<std::string> txid;
	for(const auto &pt : m_pending_tx)
		txid.push_back(epee::string_tools::pod_to_hex(cryptonote::get_transaction_hash(pt.tx)));
	return txid;
}

bool PendingTransactionImpl::commit(const std::string &filename, bool overwrite)
{

	LOG_PRINT_L3("m_pending_tx size: " << m_pending_tx.size());

	try
	{
		// Save tx to file
		if(!filename.empty())
		{
			boost::system::error_code ignore;
			bool tx_file_exists = boost::filesystem::exists(filename, ignore);
			if(tx_file_exists && !overwrite)
			{
				m_errorString = string(tr("Attempting to save transaction to file, but specified file(s) exist. Exiting to not risk overwriting. File:")) + filename;
				m_status = Status_Error;
				LOG_ERROR(m_errorString);
				return false;
			}
			bool r = m_wallet.m_wallet->save_tx(m_pending_tx, filename);
			if(!r)
			{
				m_errorString = tr("Failed to write transaction(s) to file");
				m_status = Status_Error;
			}
			else
			{
				m_status = Status_Ok;
			}
		}
		// Commit tx
		else
		{
			m_wallet.pauseRefresh();
			while(!m_pending_tx.empty())
			{
				auto &ptx = m_pending_tx.back();
				m_wallet.m_wallet->commit_tx(ptx);
				// if no exception, remove element from vector
				m_pending_tx.pop_back();
			} // TODO: extract method;
		}
	}
	catch(const tools::error::daemon_busy &)
	{
		// TODO: make it translatable with "tr"?
		m_errorString = tr("daemon is busy. Please try again later.");
		m_status = Status_Error;
	}
	catch(const tools::error::no_connection_to_daemon &)
	{
		m_errorString = tr("no connection to daemon. Please make sure daemon is running.");
		m_status = Status_Error;
	}
	catch(const tools::error::tx_rejected &e)
	{
		std::ostringstream writer(m_errorString);
		writer << (boost::format(tr("transaction %s was rejected by daemon with status: ")) % get_transaction_hash(e.tx())) << e.status();
		std::string reason = e.reason();
		m_status = Status_Error;
		m_errorString = writer.str();
		if(!reason.empty())
			m_errorString += string(tr(". Reason: ")) + reason;
	}
	catch(const std::exception &e)
	{
		m_errorString = string(tr("Unknown exception: ")) + e.what();
		m_status = Status_Error;
	}
	catch(...)
	{
		m_errorString = tr("Unhandled exception");
		LOG_ERROR(m_errorString);
		m_status = Status_Error;
	}

	m_wallet.startRefresh();
	return m_status == Status_Ok;
}

uint64_t PendingTransactionImpl::amount() const
{
	uint64_t result = 0;
	for(const auto &ptx : m_pending_tx)
	{
		for(const auto &dest : ptx.dests)
		{
			result += dest.amount;
		}
	}
	return result;
}

uint64_t PendingTransactionImpl::dust() const
{
	uint64_t result = 0;
	for(const auto &ptx : m_pending_tx)
	{
		result += ptx.dust;
	}
	return result;
}

uint64_t PendingTransactionImpl::fee() const
{
	uint64_t result = 0;
	for(const auto &ptx : m_pending_tx)
	{
		result += ptx.fee;
	}
	return result;
}

uint64_t PendingTransactionImpl::txCount() const
{
	return m_pending_tx.size();
}

std::vector<uint32_t> PendingTransactionImpl::subaddrAccount() const
{
	std::vector<uint32_t> result;
	for(const auto &ptx : m_pending_tx)
		result.push_back(ptx.construction_data.subaddr_account);
	return result;
}

std::vector<std::set<uint32_t>> PendingTransactionImpl::subaddrIndices() const
{
	std::vector<std::set<uint32_t>> result;
	for(const auto &ptx : m_pending_tx)
		result.push_back(ptx.construction_data.subaddr_indices);
	return result;
}
}
