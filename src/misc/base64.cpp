#include <cctype>

#include <agz-utils/misc/base64.h>

// ref: http://www.adp-gmbh.ch/cpp/common/base64.html (restyled)

namespace agz::misc
{

    namespace
    {
        const std::string_view base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        bool is_base64(unsigned char c)
        {
            return (std::isalnum(c) || (c == '+') || (c == '/'));
        }
    }

    std::string encode_base64(const void *data, size_t byte_size)
    {
        std::string ret;

        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        auto buf = static_cast<const unsigned char *>(data);

        int i = 0;
        while(byte_size--)
        {
            char_array_3[i++] = *buf++;
            if(i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                                  ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                                  ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; i < 4; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if(i)
        {
            for(int j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                              ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                              ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(int j = 0; j < i + 1; j++)
                ret += base64_chars[char_array_4[j]];

            while(i++ < 3)
                ret += '=';
        }

        return ret;
    }

    std::vector<unsigned char> decode_base64(std::string_view str)
    {
        int in_len = static_cast<int>(str.size());
        int in_ = 0;

        unsigned char char_array_4[4], char_array_3[3];

        std::vector<unsigned char> ret;

        int i = 0;
        while(in_len-- && (str[in_] != '=') && is_base64(str[in_]))
        {
            char_array_4[i++] = str[in_]; in_++;
            if(i == 4)
            {
                for(i = 0; i < 4; i++)
                    char_array_4[i] = static_cast<unsigned char>(
                        base64_chars.find(char_array_4[i]));

                char_array_3[0] = (char_array_4[0] << 2) +
                                  ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) +
                                  ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +
                                  char_array_4[3];

                for(i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if(i)
        {
            for(int j = i; j < 4; j++)
                char_array_4[j] = 0;

            for(int j = 0; j < 4; j++)
                char_array_4[j] = static_cast<unsigned char>(
                    base64_chars.find(char_array_4[j]));

            char_array_3[0] = (char_array_4[0] << 2) +
                              ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) +
                              ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +
                              char_array_4[3];

            for(int j = 0; (j < i - 1); j++)
                ret.push_back(char_array_3[j]);
        }

        return ret;
    }

} // namespace agz::misc
