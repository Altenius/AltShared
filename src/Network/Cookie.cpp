#include "Network/Cookie.h"
#include "StringUtil.h"
#include <algorithm>
#include <vector>


Cookie Cookie::parse(const std::string &value, const std::string &domain,
                     const std::string &path)
{
    Cookie cookie;
    cookie.domain_ = domain;

    if (path == "/")
    {
        cookie.path_ = "/";
    }
    else
    {
        size_t lastSlash = path.find_last_of('/');
        cookie.path_ = path.substr(0, lastSlash);
    }

    size_t sEnd = value.find(';');
    bool extra = true;
    if (sEnd == std::string::npos)
    {
        extra = false;
        sEnd = value.size();
    }

    std::string cookiePair = value.substr(0, sEnd);
    size_t nameEnd = cookiePair.find('=');
    if (nameEnd == std::string::npos)
    { // invalid
        return cookie;
    }

    cookie.name_ = value.substr(0, nameEnd);
    cookie.value_ = value.substr(nameEnd + 1, sEnd - nameEnd - 1);
    StringUtil::trim(cookie.name_);
    StringUtil::trim(cookie.value_);

    if (!cookie.valid())
    {
        return cookie;
    }

    if (extra)
    {
        size_t last = sEnd;
        do
        {
            sEnd = value.find(';', last + 1);
            if (sEnd == std::string::npos)
            {
                sEnd = value.size();
            }

            std::string avString = value.substr(last + 1, sEnd - last - 1);
            StringUtil::trim(avString);
            std::string lAvString = StringUtil::lower(avString);
            if (lAvString == "httponly")
            {
                cookie.httpOnly_ = true;
            }
            else if (lAvString == "secure")
            {
                cookie.secure_ = true;
            }
            else
            {
                size_t avSep = avString.find('=');
                if (avSep != std::string::npos)
                { // else it's not supported
                    std::string avName =
                        StringUtil::lower(avString.substr(0, avSep));
                    std::string avValue =
                        StringUtil::lower(avString.substr(avSep + 1));
                    StringUtil::rtrim(avName);  // the left was trimmed earlier
                    StringUtil::ltrim(avValue); // ^  right

                    if (avName == "expires")
                    {
                        if (cookie.expires_.time_since_epoch() ==
                            std::chrono::system_clock::duration::zero())
                        {
                            if (!cookie.parseDate(avValue))
                            {
                                // invalid date
                            }
                        }
                    }
                    else if (avName == "max-age")
                    {
                        unsigned long time;
                        if (StringUtil::stoi(avValue, time))
                        {
                            cookie.expires_ =
                                std::chrono::system_clock::now() +
                                std::chrono::system_clock::duration(time);
                        }
                    }
                    else if (avName == "domain")
                    {
                        if (avValue.front() != '.')
                        {
                            avValue.insert(avValue.begin(), '.');
                        }
                        cookie.domain_ = avValue;
                    }
                    else if (avName == "path")
                    {
                        cookie.path_ = StringUtil::absolutePath(avValue);
                    }
                }
            }


            last = sEnd;
        } while (last != value.size());
    }

    return cookie;
}


Cookie::Cookie(const std::string &name, const std::string &value,
               const std::string &domain, const std::string &path,
               bool httpOnly, bool secure, TimeType expires)
    : name_(name), value_(value), domain_(domain), path_(path),
      httpOnly_(httpOnly), secure_(secure), expires_(expires)
{
}


Cookie::Cookie() : expires_(std::chrono::duration<unsigned long>(0))
{
}


unsigned char Cookie::parseMonth(const std::string &abvro)
{
    std::string abvr = StringUtil::lower(abvro);
    if (abvr == "jan")
    {
        return 1;
    }
    else if (abvr == "feb")
    {
        return 2;
    }
    else if (abvr == "mar")
    {
        return 3;
    }
    else if (abvr == "apr")
    {
        return 4;
    }
    else if (abvr == "may")
    {
        return 5;
    }
    else if (abvr == "jun")
    {
        return 6;
    }
    else if (abvr == "jul")
    {
        return 7;
    }
    else if (abvr == "aug")
    {
        return 8;
    }
    else if (abvr == "sep")
    {
        return 9;
    }
    else if (abvr == "oct")
    {
        return 10;
    }
    else if (abvr == "nov")
    {
        return 11;
    }
    else if (abvr == "dec")
    {
        return 12;
    }

    return 0;
}


bool is_delimeter(char c)
{
    return (c == 0x09 || (c >= 0x20 && c <= 0x2F) || (c >= 0x3B && c <= 0x40) ||
            (c >= 0x5B && c <= 0x60) || (c >= 0x7B && c <= 0x7E));
}


bool parseTime(const std::string &token, unsigned int &hour, unsigned int &min,
               unsigned int &sec)
{
    bool fhour = false, fmin = false;
    std::string n;
    for (auto it = token.begin(); it != token.end(); it++)
    {
        if (*it == ':')
        {
            if (n.empty())
            {
                return false;
            }
            if (fmin)
            {
                return false;
            }
            else if (fhour)
            {
                fmin = true;
                StringUtil::stoi(n, min);
            }
            else
            {
                fhour = true;
                StringUtil::stoi(n, hour);
            }
            n.clear();
        }
        else if (std::isdigit(*it))
        {
            n += *it;
        }
        else
        {
            return false;
        }
    }

    if (!n.empty() && fmin)
    {
        StringUtil::stoi(n, sec);
        return true;
    }

    return false;
}


bool Cookie::parseDate(const std::string &date)
{
    std::vector<std::string> tokens;
    std::string last;

    std::for_each(date.begin(), date.end(), [&tokens, &last](char c) {
        if (!is_delimeter(c))
        {
            last += c;
        }
        else if (!last.empty())
        {
            tokens.push_back(last);
            last.clear();
        }
    });

    if (!last.empty())
    {
        tokens.push_back(last);
    }

    unsigned int hour, min, sec, day, month, year;

    bool found_time = false, found_day_of_month = false, found_month = false,
         found_year = false;
    for (std::string token : tokens)
    {
        if (!found_time && parseTime(token, hour, min, sec))
        {
            if (hour > 23 || min > 59 || sec > 59)
            {
                return false;
            }
            found_time = true;
            continue;
        }
        if (!found_day_of_month && StringUtil::stoi(token, day))
        {
            if (day < 1 || day > 31)
            {
                return false;
            }
            found_day_of_month = true;
            continue;
        }
        if (!found_month && (month = parseMonth(token)) != 0)
        {
            found_month = true;
            continue;
        }
        if (!found_year && StringUtil::stoi(token, year))
        {
            found_year = true;
            if (year >= 70 && year <= 99)
            {
                year += 1900;
            }
            else if (year >= 0 && year <= 69)
            {
                year += 2000;
            }
            if (year < 1601)
            {
                return false;
            }
            continue;
        }
    }

    if (found_time && found_day_of_month && found_month && found_year)
    {
        std::tm timeinfo = std::tm();

        timeinfo.tm_gmtoff = 0;
        timeinfo.tm_zone = "UTC";
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_hour = hour - 1;
        timeinfo.tm_min = min;
        timeinfo.tm_sec = sec;
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mday = day;

        std::time_t tt = std::mktime(&timeinfo);
        expires_ = std::chrono::system_clock::from_time_t(tt);
        return true;
    }

    return false;
}


bool Cookie::usable(const std::string &domain, const std::string &path)
{
    // if (expired()) { // this should be checked before calling this function
    // return false;
    // }

    if (domain_.front() == '.')
    {
        if (domain_.size() > domain.size())
        {
            return false;
        }
        if (domain.substr(domain.size() - domain_.size()) != domain_)
        {
            return false;
        }
    }
    else if (domain != domain_)
    {
        return false;
    }

    if (path.size() < path_.size())
    {
        return false;
    }

    return path.substr(0, path_.size()) == path_;
}


CookieJar::CookieJar()
{
}


void CookieJar::parse(const std::string &str, const std::string &domain,
                      const std::string &path)
{
    Cookie c = Cookie::parse(str, domain, path);

    if (c.valid())
    {
        cookies_.push_back(c);
    }
}


std::string CookieJar::getCookies(const std::string &domain,
                                  const std::string &path)
{
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();

    for (auto it = cookies_.begin(); it != cookies_.end(); it++)
    {
        if (it->expired(now))
        {
            cookies_.erase(it);
            it--;
            continue;
        }
        if (it->usable(domain, path))
        {
            if (it != cookies_.begin())
            {
                ss << "; ";
            }
            ss << it->name() << '=' << it->value();
        }
    }

    return ss.str();
}
