#pragma once

namespace {

    class StringStream {
    public:
        operator std::string() const { return m_buffer.str(); }

        template<typename T>
        StringStream& operator<<(const T& toAdd)
        {
            m_buffer << toAdd;
            return *this;
        }

    private:
        std::ostringstream m_buffer;
    };

    std::string quote(const std::string &str) {
        return StringStream() << '"' << str << '"';
    }
}