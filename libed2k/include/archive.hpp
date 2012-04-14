#ifndef __LIBED2K_ARCHIVE__
#define __LIBED2K_ARCHIVE__

#include <iostream>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits.hpp>
#include "error_code.hpp"

namespace libed2k
{
    namespace archive
    {

        class access
        {
        public:
            // pass calls to users's class implementation
            template<class Archive, class T>
            static void member_save(Archive & ar, T& t)
            {
                t.save(ar);
            }

            template<class Archive, class T>
            static void member_load(Archive& ar, T& t)
            {
                t.load(ar);
            }
        };

        template<class Archive, class T>
        struct member_saver
        {
            static void invoke(Archive & ar,const T & t)
            {
                access::member_save(ar, t);
            }
        };

        template<class Archive, class T>
        struct member_loader
        {
            static void invoke(Archive & ar,T & t)
            {
                access::member_load(ar, t);
            }
        };

        template<class Archive, class T>
        inline void split_member( Archive & ar, T & t)
        {
            typedef BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
                BOOST_DEDUCED_TYPENAME Archive::is_saving,
                boost::mpl::identity<member_saver<Archive, T> >,
                boost::mpl::identity<member_loader<Archive, T> >
            >::type typex;
            typex::invoke(ar, t);
        }

        // split member function serialize funcition into save/load
        #define LIBED2K_SERIALIZATION_SPLIT_MEMBER() \
        template<class Archive>                      \
        void serialize(Archive& ar)\
        {                                            \
            libed2k::archive::split_member(ar, *this); \
        }

        class ed2k_oarchive
        {
        public:
            typedef boost::mpl::bool_<false> is_loading;
            typedef boost::mpl::bool_<true> is_saving;

            ed2k_oarchive(std::ostream& container) : m_container(container)
            {
            }

            std::ostream& container()
            {
                return (m_container);
            }

            template<typename T>
            ed2k_oarchive& operator<<(T& t)
            {
                serialize_impl(t);
                return *this;
            }

            ed2k_oarchive& operator <<(std::string& str)
            {
                raw_write(str.c_str(), str.size());
                return *this;
            }

            // special const
            ed2k_oarchive& operator <<(const std::string& str)
            {
                raw_write(str.c_str(), str.size());
                return *this;
            }

            template<typename T>
            ed2k_oarchive& operator&(T& t)
            {
                *this << t;
                return (*this);
            }

            template<typename T>
            void raw_write(T p, size_t nSize)
            {
                m_container.write(p, nSize);
                if (!m_container.good())
                {
                    throw libed2k::libed2k_exception(libed2k::errors::unexpected_ostream_error);
                }
            }

        private:
            // Fundamental types simple read as raw binary block
            template<typename T>
            inline void serialize_impl(T & val, typename boost::enable_if<boost::is_fundamental<T> >::type* dummy = 0)
            {
                raw_write(reinterpret_cast<const char*>(&val), sizeof(T));
            }

            //Classes need to be serialize using their special method
            template<typename T>
            inline void serialize_impl(T & val,  typename boost::enable_if<boost::is_class<T> >::type* dummy = 0)
            {
                val.serialize(*this);
            }

            std::ostream& m_container;
        };

        class ed2k_iarchive
        {
        public:
            typedef boost::mpl::bool_<true> is_loading;
            typedef boost::mpl::bool_<false> is_saving;


            ed2k_iarchive(std::istream& container) : m_container(container)
            {
            }

            std::istream& container()
            {
                return (m_container);
            }

            template<typename T>
            ed2k_iarchive& operator>>(T& t)
            {
                deserialize_impl(t);
                return (*this);
            }

            template<typename T>
            ed2k_iarchive& operator&(T& t)
            {
                *this >> t;
                return (*this);
            }

            template<typename T>
            void raw_read(T t, size_t nSize)
            {
                m_container.read(t, nSize);

                if (!m_container.good())
                {
                    throw libed2k::libed2k_exception(libed2k::errors::unexpected_istream_error);
                }
            }

            // simple pass some data
            void shift(size_t nSize)
            {
                m_container.seekg(nSize, std::ios_base::cur);

                if (!m_container.good())
                {
                    throw libed2k::libed2k_exception(libed2k::errors::unexpected_istream_error);
                }
            }

            ed2k_iarchive& operator>>(std::string& str)
            {
                //uint16_t nSize = static_cast<uint16_t>(str.size());
                //*this >> nSize;
                size_t nSize = str.size();

                if (nSize != 0)
                {
			        std::vector<char> v(nSize);
                    raw_read(&v[0], nSize);
                    str.assign(&v[0], nSize);
                }

                return *this;
            }
        private:
            std::istream& m_container;

            template<typename T>
            inline void deserialize_impl(T & val, typename boost::enable_if<boost::is_fundamental<T> >::type* dummy = 0)
            {
                raw_read(reinterpret_cast<char*>(&val), sizeof(T));
            }
           
            //Classes
            template<typename T>
            inline void deserialize_impl(T & val, typename boost::enable_if<boost::is_class<T> >::type* dummy = 0)
            {
                val.serialize(*this);
            }

        };
    }
}

#endif //__LIBED2K_ARCHIVE__
