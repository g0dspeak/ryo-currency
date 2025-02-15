# Copyright (c) 2020, pasta Currency Project
# Portions copyright (c) 2014-2018, The Monero Project
#
# Portions of this file are available under BSD-3 license. Please see ORIGINAL-LICENSE for details
# All rights reserved.
# 
# Authors and copyright holders give permission for following:
#
# 1. Redistribution and use in source and binary forms WITHOUT modification.
#
# 2. Modification of the source form for your own personal use. 
# 
# As long as the following conditions are met:
# 
# 3. You must not distribute modified copies of the work to third parties. This includes 
#    posting the work online, or hosting copies of the modified work for download.
#
# 4. Any derivative version of this work is also covered by this license, including point 8.
#
# 5. Neither the name of the copyright holders nor the names of the authors may be
#    used to endorse or promote products derived from this software without specific
#    prior written permission.
#
# 6. You agree that this licence is governed by and shall be construed in accordance 
#    with the laws of England and Wales.
#
# 7. You agree to submit all disputes arising out of or in connection with this licence 
#    to the exclusive jurisdiction of the Courts of England and Wales.
#
# Authors and copyright holders agree that:
#
# 8. This licence expires and the work covered by it is released into the 
#    public domain on 1st of February 2021
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(device_sources
  device.cpp
  device_default.cpp
  log.cpp
  )

if(PCSC_FOUND)
  set(device_sources ${device_sources} device_ledger.cpp)
endif()

set(device_headers
  device.hpp
  device_default.hpp
  log.hpp
 )

if(PCSC_FOUND)
  set(device_headers ${device_headers} device_ledger.hpp)
endif()

set(device_private_headers)


if(PER_BLOCK_CHECKPOINT)
  set(Blocks "blocks")
else()
  set(Blocks "")
endif()

pasta_private_headers(device
  ${device_private_headers})

pasta_add_library(device
  ${device_sources}
  ${device_headers}
  ${device_private_headers})

target_link_libraries(device
  PUBLIC
    ${PCSC_LIBRARIES}
    cncrypto
    ringct_basic
    ${OPENSSL_CRYPTO_LIBRARIES}
    fmt::fmt-header-only
  PRIVATE
    ${Blocks}
    ${EXTRA_LIBRARIES}
    fmt::fmt-header-only)
