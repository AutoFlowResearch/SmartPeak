// TODO: Add copyright

#include <SmartPeak/core/CastValue.h>

namespace SmartPeak
{
  CastValue& CastValue::operator=(const CastValue& other)
  {
    if (this == &other)
      return *this;
    switch (other.tag_) {
      case Type::UNKNOWN:
      case Type::STRING:
        setTagAndData(other.tag_, other.s_);
        break;
      case Type::UNINITIALIZED:
      case Type::BOOL:
        setTagAndData(other.tag_, other.b_);
        break;
      case Type::FLOAT:
        setTagAndData(other.tag_, other.f_);
        break;
      case Type::INT:
        setTagAndData(other.tag_, other.i_);
        break;
      case Type::LONG_INT:
        setTagAndData(other.tag_, other.li_);
        break;
      case Type::BOOL_LIST:
        setTagAndData(other.tag_, other.bl_);
        break;
      case Type::FLOAT_LIST:
        setTagAndData(other.tag_, other.fl_);
        break;
      case Type::INT_LIST:
        setTagAndData(other.tag_, other.il_);
        break;
      case Type::STRING_LIST:
        setTagAndData(other.tag_, other.sl_);
        break;
      default:
        throw "Tag type not managed in copy assignment constructor. Implement it.";
    }
    return *this;
  }

  CastValue& CastValue::operator=(CastValue&& other)
  {
    if (this == &other)
      return *this;
    clear();
    switch (other.tag_) {
      case Type::UNKNOWN:
      case Type::STRING:
        new (&s_) std::string(std::move(other.s_));
        is_clear_ = false;
        break;
      case Type::UNINITIALIZED:
      case Type::BOOL:
        b_ = other.b_;
        break;
      case Type::FLOAT:
        f_ = other.f_;
        break;
      case Type::INT:
        i_ = other.i_;
        break;
      case Type::LONG_INT:
        li_ = other.li_;
        break;
      case Type::BOOL_LIST:
        new (&bl_) std::vector<bool>(std::move(other.bl_));
        is_clear_ = false;
        break;
      case Type::FLOAT_LIST:
        new (&fl_) std::vector<float>(std::move(other.fl_));
        is_clear_ = false;
        break;
      case Type::INT_LIST:
        new (&il_) std::vector<int>(std::move(other.il_));
        is_clear_ = false;
        break;
      case Type::STRING_LIST:
        new (&sl_) std::vector<std::string>(std::move(other.sl_));
        is_clear_ = false;
        break;
      default:
        throw "Tag type not managed in move assignment operator. Implement it.";
    }
    tag_ = other.tag_;
    other.tag_ = Type::UNINITIALIZED;
    other.is_clear_ = true;
    other.b_ = false;
    return *this;
  }

  CastValue& CastValue::operator=(const bool data)
  {
    setTagAndData(Type::BOOL, data);
    return *this;
  }

  CastValue& CastValue::operator=(const float data)
  {
    setTagAndData(Type::FLOAT, data);
    return *this;
  }

  CastValue& CastValue::operator=(const int data)
  {
    setTagAndData(Type::INT, data);
    return *this;
  }

  CastValue& CastValue::operator=(const long int data)
  {
    setTagAndData(Type::LONG_INT, data);
    return *this;
  }

  CastValue& CastValue::operator=(const char *data)
  {
    setTagAndData(Type::STRING, std::move(std::string(data)));
    return *this;
  }

  CastValue& CastValue::operator=(const std::string& data)
  {
    setTagAndData(Type::STRING, data);
    return *this;
  }

  CastValue& CastValue::operator=(const std::vector<bool>& data)
  {
    setTagAndData(Type::BOOL_LIST, data);
    return *this;
  }

  CastValue& CastValue::operator=(const std::vector<float>& data)
  {
    setTagAndData(Type::FLOAT_LIST, data);
    return *this;
  }

  CastValue& CastValue::operator=(const std::vector<int>& data)
  {
    setTagAndData(Type::INT_LIST, data);
    return *this;
  }

  CastValue& CastValue::operator=(const std::vector<std::string>& data)
  {
    setTagAndData(Type::STRING_LIST, data);
    return *this;
  }

  bool CastValue::operator<(const CastValue& other) const
  {
    return is_less_than(other, true);
  }

  bool CastValue::operator>(const CastValue& other) const
  {
    return is_greater_than(other, true);
  }

  bool CastValue::operator==(const CastValue& other) const
  {
    return is_equal_to(other, true);
  }

  bool CastValue::operator<=(const CastValue& other) const
  {
    return is_less_than(other, true) || is_equal_to(other, true);
  }

  bool CastValue::operator>=(const CastValue& other) const
  {
    return is_greater_than(other, true) || is_equal_to(other, true);
  }

  bool CastValue::is_less_than(const CastValue& other, const bool case_sensitive) const
  {
    if (tag_ != other.tag_) {
      LOGE << "CastValue: Comparing data of different types";
      return true;
    }

    switch (tag_) {
    case Type::STRING:
      {
        if (!case_sensitive) {
          std::string a_lowercase, b_lowercase;
          a_lowercase.resize(s_.size());
          b_lowercase.resize(other.s_.size());
          std::transform(s_.begin(), s_.end(), a_lowercase.begin(), ::tolower);
          std::transform(other.s_.begin(), other.s_.end(), b_lowercase.begin(), ::tolower);
          return a_lowercase.compare(b_lowercase) < 0;
        }
        return s_.compare(other.s_) < 0;
      }
    case Type::UNINITIALIZED:
    case Type::BOOL:
      return b_ < other.b_;
    case Type::FLOAT:
      return f_ < other.f_;
    case Type::INT:
      return i_ < other.i_;
    case Type::LONG_INT:
      return li_ < other.li_;
    default:
      LOGE << "Tag type cannot be compared";
      return true;
    }
  }

  bool CastValue::is_greater_than(const CastValue& other, const bool case_sensitive) const
  {
    if (tag_ != other.tag_) {
      LOGE << "CastValue: Comparing data of different types";
      return true;
    }

    switch (tag_) {
    case Type::STRING:
    {
      if (!case_sensitive) {
        std::string a_lowercase, b_lowercase;
        a_lowercase.resize(s_.size());
        b_lowercase.resize(other.s_.size());
        std::transform(s_.begin(), s_.end(), a_lowercase.begin(), ::tolower);
        std::transform(other.s_.begin(), other.s_.end(), b_lowercase.begin(), ::tolower);
        return a_lowercase.compare(b_lowercase) > 0;
      }
      return s_.compare(other.s_) > 0;
    }
    case Type::UNINITIALIZED:
    case Type::BOOL:
      return b_ > other.b_;
    case Type::FLOAT:
      return f_ > other.f_;
    case Type::INT:
      return i_ > other.i_;
    case Type::LONG_INT:
      return li_ > other.li_;
    default:
      LOGE << "Tag type cannot be compared";
      return true;
    }
  }

  bool CastValue::is_equal_to(const CastValue& other, const bool case_sensitive) const
  {
    if (tag_ != other.tag_) {
      LOGE << "CastValue: Comparing data of different types";
      return false;
    }

    switch (tag_) {
    case Type::STRING:
    {
      if (!case_sensitive) {
        std::string a_lowercase, b_lowercase;
        a_lowercase.resize(s_.size());
        b_lowercase.resize(other.s_.size());
        std::transform(s_.begin(), s_.end(), a_lowercase.begin(), ::tolower);
        std::transform(other.s_.begin(), other.s_.end(), b_lowercase.begin(), ::tolower);
        return a_lowercase.compare(b_lowercase) == 0;
      }
      return s_.compare(other.s_) == 0;
    }
    case Type::UNINITIALIZED:
    case Type::BOOL:
      return b_ == other.b_;
    case Type::FLOAT:
      return f_ == other.f_;
    case Type::INT:
      return i_ == other.i_;
    case Type::LONG_INT:
      return li_ == other.li_;
    default:
      LOGE << "Tag type cannot be compared";
      return true;
    }
  }

  void CastValue::clear()
  {
    if (is_clear_)
      return;

    switch (tag_) {
      case Type::UNKNOWN:
      case Type::STRING:
        s_.~basic_string();
        break;
      case Type::BOOL_LIST:
        bl_.~vector();
        break;
      case Type::FLOAT_LIST:
        fl_.~vector();
        break;
      case Type::INT_LIST:
        il_.~vector();
        break;
      case Type::STRING_LIST:
        sl_.~vector();
        break;
      default:
        // nothing to deallocate
        break;
    }

    is_clear_ = true;
  }

  CastValue::Type CastValue::getTag() const { return tag_; }

  void CastValue::setData(const bool data)
  {
    b_ = data;
  }

  void CastValue::setData(const float data)
  {
    f_ = data;
  }

  void CastValue::setData(const int data)
  {
    i_ = data;
  }

  void CastValue::setData(const long int data)
  {
    li_ = data;
  }

  void CastValue::setData(const std::string& data)
  {
    new (&s_) std::string(data);
    is_clear_ = false;
  }

  void CastValue::setData(const std::vector<bool>& data)
  {
    new (&bl_) std::vector<bool>(data);
    is_clear_ = false;
  }

  void CastValue::setData(const std::vector<float>& data)
  {
    new (&fl_) std::vector<float>(data);
    is_clear_ = false;
  }

  void CastValue::setData(const std::vector<int>& data)
  {
    new (&il_) std::vector<int>(data);
    is_clear_ = false;
  }

  void CastValue::setData(const std::vector<std::string>& data)
  {
    new (&sl_) std::vector<std::string>(data);
    is_clear_ = false;
  }
}
