/* From http://stackoverflow.com/questions/5100718/integer-to-hex-string-in-c */


template< typename T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << "0x" << std::setfill('0') << std::setw(sizeof(T)*2) << std::hex << i;
	return stream.str();
}



