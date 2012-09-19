/*

Copyright (c) 2009, Arvid Norberg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#include <string>

#include "libed2k/escape_string.hpp"
#include "libed2k/error_code.hpp"
#include "libed2k/socket.hpp"
#include "libed2k/socket_io.hpp"
#include "libed2k/address.hpp"
#include "libed2k/hasher.hpp" // for hasher

namespace libed2k
{

	std::string print_address(address const& addr)
	{
		error_code ec;
		return addr.to_string(ec);
	}

	std::string address_to_bytes(address const& a)
	{
#if LIBED2K_USE_IPV6
		if (a.is_v6())
		{
			address_v6::bytes_type b = a.to_v6().to_bytes();
			return std::string((char*)&b[0], b.size());
		}
		else
#endif
		{
			address_v4::bytes_type b = a.to_v4().to_bytes();
			return std::string((char*)&b[0], b.size());
		}
	}

	std::string print_endpoint(tcp::endpoint const& ep)
	{
		error_code ec;
		std::string ret;
		address const& addr = ep.address();
#if LIBED2K_USE_IPV6
		if (addr.is_v6())
		{
			ret += '[';
			ret += addr.to_string(ec);
			ret += ']';
			ret += ':';
			ret += to_string(ep.port()).elems;
		}
		else
#endif
		{
			ret += addr.to_string(ec);
			ret += ':';
			ret += to_string(ep.port()).elems;
		}
		return ret;
	}

	std::string print_endpoint(udp::endpoint const& ep)
	{
		return print_endpoint(tcp::endpoint(ep.address(), ep.port()));
	}

	void hash_address(address const& ip, sha1_hash& h)
	{
#if LIBED2K_USE_IPV6
		if (ip.is_v6())
		{
			address_v6::bytes_type b = ip.to_v6().to_bytes();
			h = hasher((char*)&b[0], b.size()).final();
		}
		else
#endif
		{
			address_v4::bytes_type b = ip.to_v4().to_bytes();
			h = hasher((char*)&b[0], b.size()).final();
		}
	}

}

