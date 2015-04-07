#pragma once

#include <algorithm>
#include "static_assert.h"

namespace color {
//////////////////////////////////////////////////////////////////////////

typedef unsigned char Channel;

struct BaseGray {
protected:
  enum {
    R2Y = 4899,
    G2Y = 9617,
    B2Y = 1868
  };

  struct CalculatedCoeffs {
    enum {
      ColorCount = 256,
      red_coeff   = 6968,
      green_coeff = 23434,
      blue_coeff = 32768 - red_coeff - green_coeff
    };

    int r[ColorCount];
    int g[ColorCount];
    int b[ColorCount];

    CalculatedCoeffs() {
      for (int i = 0; i < ColorCount; ++i) {
        r[i] = i * red_coeff;
        g[i] = i * green_coeff;
        b[i] = i * blue_coeff;
      }
    }

    unsigned char get(unsigned char ar, unsigned char ag, unsigned char ab) const {
      return (unsigned char)((r[ar] + g[ag] + b[ab]) >> 15);
    }
  };

  static const CalculatedCoeffs coeff;
};

template<class ChannelType>
struct GrayTmpl : public BaseGray {
  ChannelType color;

  Channel get() const {
    return color[0];
  }

  void set(Channel ch) {
    color[0] = ch;
  }
protected:

  GrayTmpl() {}

  GrayTmpl(ChannelType ptr)
    : color(ptr) {}
};

//////////////////////////////////////////////////////////////////////////
enum Channels {
  NoChannel,
  Red,
  Green,
  Blue,
  Alpha
};

template<int IntVal>
struct Int2Type {
  enum {
    value = IntVal
  };
};

template<class TypeCheckFor, class CheckType>
struct is_type {
  enum {
    value = 0
  };
};

template<class TypeCheckFor>
struct is_type<TypeCheckFor, TypeCheckFor> {
  enum {
    value = 1
  };
};

static const unsigned char NotTransparent = 255;


////
template<bool has_alpha>
struct HelpAlpha {
  static Channel get(const Channel*, int) {
    return NotTransparent;
  }
  static void set(Channel*, int, Channel) {}
};

template<>
struct HelpAlpha<true> {
  static Channel get(const Channel* channels, int index) {
    return channels[index];
  }
  static void set(Channel* channels, int index, Channel value) {
    channels[index] = value;
  }
};

////


template<int Color1, int Color2, int Color3, int Color4, class ChannelsType>
struct ColorBase {
  typedef Int2Type<Color1> Color1Type;
  typedef Int2Type<Color2> Color2Type;
  typedef Int2Type<Color3> Color3Type;
  typedef Int2Type<Color4> Color4Type;

  typedef Int2Type<Red>  RedType;
  typedef Int2Type<Green>  GreenType;
  typedef Int2Type<Blue>  BlueType;
  typedef Int2Type<Alpha>  AlphaType;
  typedef Int2Type<NoChannel> NoType;

  ChannelsType colors;

  enum ColorIndex {
    RedIndex = is_type<RedType, Color1Type>::value ? 0 :
               is_type<RedType, Color2Type>::value ? 1 :
               is_type<RedType, Color3Type>::value ? 2 :
               is_type<RedType, Color4Type>::value ? 3 : -1,

    GreenIndex =is_type<GreenType, Color1Type>::value ? 0 :
                 is_type<GreenType, Color2Type>::value ? 1 :
                 is_type<GreenType, Color3Type>::value ? 2 :
                 is_type<GreenType, Color4Type>::value ? 3 : -1,

    BlueIndex = is_type<BlueType, Color1Type>::value ? 0 :
                is_type<BlueType, Color2Type>::value ? 1 :
                is_type<BlueType, Color3Type>::value ? 2 :
                is_type<BlueType, Color4Type>::value ? 3 : -1,

    AlphaIndex =is_type<AlphaType, Color1Type>::value ? 0 :
                 is_type<AlphaType, Color2Type>::value ? 1 :
                 is_type<AlphaType, Color3Type>::value ? 2 :
                 is_type<AlphaType, Color4Type>::value ? 3 : -1
  };

  STATIC_ASSERT((RedIndex != -1), no_red_channel);
  STATIC_ASSERT((GreenIndex != -1), no_green_channel);
  STATIC_ASSERT((BlueIndex != -1), no_blue_channel);

  ColorBase() {}

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> ColorBase(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    apply(other);
  }

  template<class OtherChannelsType> ColorBase(const GrayTmpl<OtherChannelsType>& gray) {
    apply(gray);
  }

  void swap(ColorIndex index1, ColorIndex index2) {
    std::swap(colors[index1], colors[index2]);
  }

  Channel red() const {
    return colors[RedIndex];
  }

  Channel green() const {
    return colors[GreenIndex];
  }

  Channel blue() const {
    return colors[BlueIndex];
  }

  Channel alpha() const {
    return HelpAlpha < AlphaIndex != -1 > ::get(colors, AlphaIndex);
  }

  void setRed(Channel r) {
    colors[RedIndex] = r;
  }

  void setGreen(Channel g) {
    colors[GreenIndex] = g;
  }

  void setBlue(Channel b) {
    colors[BlueIndex] = b;
  }

  void setAlpha(Channel a) {
    HelpAlpha<AlphaIndex != -1>::set(colors, AlphaIndex, a);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  bool operator ==(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) const {
    return red() == other.red() &&
           green() == other.green() &&
           blue() == other.blue() &&
           alpha() == other.alpha();
  }

protected:
  ColorBase(ChannelsType ptr)
    : colors(ptr) {}

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> ColorBase(ChannelsType ptr, const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other)
    : colors(ptr) {
    apply(other);
  }

  template<class OtherChannelsType> ColorBase(ChannelsType ptr, const GrayTmpl<OtherChannelsType>& gray)
    : colors(ptr) {
    apply(gray);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  void apply(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    setRed(other.red());
    setGreen(other.green());
    setBlue(other.blue());
    setAlpha(other.alpha());
  }

  template<class OtherChannelsType>
  void apply(const GrayTmpl<OtherChannelsType>& gray) {
    setRed(gray.get());
    setGreen(gray.get());
    setBlue(gray.get());
    setAlpha(NotTransparent);
  }

  void apply(Channel col1, Channel col2, Channel col3, Channel col4) {
    colors[0] = col1;
    colors[1] = col2;
    colors[2] = col3;
    colors[3] = col4;
  }

  void apply(Channel col1, Channel col2, Channel col3) {
    colors[0] = col1;
    colors[1] = col2;
    colors[2] = col3;
  }
};

//////////////////////////////////////////////////////////////////////////

template<int Color1, int Color2, int Color3>
struct Color3Tmpl : public ColorBase<Color1, Color2, Color3, NoChannel, Channel[3]> {
  typedef ColorBase<Color1, Color2, Color3, NoChannel, Channel[3]> BaseType;
  typedef Color3Tmpl<Color1, Color2, Color3> ThisType;

  STATIC_ASSERT((ThisType::AlphaIndex == -1), no_alpha_channel);

  Color3Tmpl() {
    STATIC_ASSERT((sizeof(ThisType) == sizeof(Channel) * 3), size_should_be_3);
  }

  Color3Tmpl(Channel col1, Channel col2, Channel col3) {
    STATIC_ASSERT((sizeof(ThisType) == sizeof(Channel) * 3), size_should_be_3);

    BaseType::apply(col1, col2, col3);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> Color3Tmpl(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other)
    : BaseType(other) {}

  template<class OtherChannelType> Color3Tmpl(const GrayTmpl<OtherChannelType>& gray)
    : BaseType(gray) {}

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  ThisType operator =(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    apply(other);
    return *this;
  }
};


template<int Color1, int Color2, int Color3, int Color4>
struct Color4Tmpl : public ColorBase<Color1, Color2, Color3, Color4, Channel[4]> {
  typedef ColorBase<Color1, Color2, Color3, Color4, Channel[4]> BaseType;
  typedef Color4Tmpl<Color1, Color2, Color3, Color4> ThisType;

  STATIC_ASSERT((ThisType::AlphaIndex != -1), no_alpha_channel);

  Color4Tmpl() {
    STATIC_ASSERT((sizeof(ThisType) == sizeof(Channel) * 4), size_should_be_4);
  }

  Color4Tmpl(Channel col1, Channel col2, Channel col3, Channel col4) {
    STATIC_ASSERT((sizeof(ThisType) == sizeof(Channel) * 4), size_should_be_4);

    BaseType::apply(col1, col2, col3, col4);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> Color4Tmpl(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other)
    : BaseType(other) {}

  template<class OtherChannelType> Color4Tmpl(const GrayTmpl<OtherChannelType>& gray)
    : BaseType(gray) {}

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  ThisType operator =(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    apply(other);
    return *this;
  }
};

//////////////////////////////////////////////////////////////////////////

template<int Color1, int Color2, int Color3, class ChannelsType>
struct Color3RefTmpl : public ColorBase<Color1, Color2, Color3, NoChannel, ChannelsType> {
  typedef ColorBase<Color1, Color2, Color3, NoChannel, ChannelsType> BaseType;
  typedef Color3RefTmpl<Color1, Color2, Color3, ChannelsType> ThisType;

  Color3RefTmpl(ChannelsType colors)
    : BaseType(colors) {}

  Color3RefTmpl(ChannelsType colors, Channel col1, Channel col2, Channel col3)
    : BaseType(colors) {
    BaseType::apply(col1, col2, col3);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  ThisType operator =(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    Color3Tmpl<OtherColor1, OtherColor2, OtherColor3> copy = other;
    apply(copy);

    return *this;
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> Color3RefTmpl(ChannelsType colors, const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other)
    : BaseType(colors, other) {}

  template<class OtherChannelsType> Color3RefTmpl(ChannelsType colors, const GrayTmpl<OtherChannelsType>& gray)
    : BaseType(colors, gray) {}
};

template<int Color1, int Color2, int Color3, int Color4, class ChannelsType>
struct Color4RefTmpl : public ColorBase<Color1, Color2, Color3, Color4, ChannelsType> {
  typedef ColorBase<Color1, Color2, Color3, Color4, ChannelsType> BaseType;
  typedef Color4RefTmpl<Color1, Color2, Color3, Color4, ChannelsType> ThisType;

  Color4RefTmpl(ChannelsType colors)
    : BaseType(colors) {}

  Color4RefTmpl(ChannelsType colors, Channel col1, Channel col2, Channel col3, Channel col4)
    : BaseType(colors) {
    BaseType::apply(col1, col2, col3, col4);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  ThisType operator =(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    apply(other);
    return *this;
  }

  //private:
  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> Color4RefTmpl(ChannelsType colors, const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other)
    : BaseType(colors, other) {}

  template<class OtherChannelsType> Color4RefTmpl(ChannelsType colors, const GrayTmpl<OtherChannelsType>& gray)
    : BaseType(colors, gray) {}
};

//////////////////////////////////////////////////////////////////////////

struct Gray : GrayTmpl<Channel[1]> {
public:
  Gray() {}

  Gray(Channel value) {
    set(value);
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> Gray(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    set(coeff.get(other.red(), other.green(), other.blue()));
  }

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  Gray operator =(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    set(coeff.get(other.red(), other.green(), other.blue()));
    return *this;
  }
};

template<class ChannelType>
struct GrayRefImpl : GrayTmpl<ChannelType> {
  typedef GrayTmpl<ChannelType> BaseType;
  typedef GrayRefImpl<ChannelType> ThisType;
public:
  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType> GrayRefImpl(ChannelType ptr, const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other)
    : BaseType(ptr) {
    BaseType::set(BaseGray::coeff.get(other.red(), other.green(), other.blue()));
  }

  template<class OtherChannelsType> GrayRefImpl(ChannelType ptr, const GrayTmpl<OtherChannelsType>& other)
    : BaseType(ptr) {
    BaseType::set(other.get());
  }

  GrayRefImpl(ChannelType ptr)
    : BaseType(ptr) {}

  template<int OtherColor1, int OtherColor2, int OtherColor3, int OtherColor4, class OtherChannelsType>
  ThisType operator =(const ColorBase<OtherColor1, OtherColor2, OtherColor3, OtherColor4, OtherChannelsType>& other) {
    set(BaseGray::coeff.get(other.red(), other.green(), other.blue()));
    return *this;
  }

  template<class OtherChannelsType>
  ThisType operator =(const GrayTmpl<OtherChannelsType>& other) {
    set(other.get());
    return *this;
  }
};

//////////////////////////////////////////////////////////////////////////

typedef GrayRefImpl<Channel*> GrayRef;
typedef GrayRefImpl<const Channel*> GrayConstRef;


//////////////////////////////////////////////////////////////////////////


typedef Color3Tmpl<Red, Green, Blue> Rgb;
typedef Color3Tmpl<Blue, Green, Red> Bgr;


typedef Color4Tmpl<Red, Green, Blue, Alpha> Rgba;
typedef Color4Tmpl<Alpha, Red, Green, Blue> Argb;

typedef Color4Tmpl<Blue, Green, Red, Alpha> Bgra;
typedef Color4Tmpl<Alpha, Blue, Green, Red> Abgr;



//////////////////////////////////////////////////////////////////////////

typedef Color3RefTmpl<Red, Green, Blue, Channel*> RgbRef;
typedef Color3RefTmpl<Blue, Green, Red, Channel*> BgrRef;

typedef Color3RefTmpl<Red, Green, Blue, const Channel*> RgbConstRef;
typedef Color3RefTmpl<Blue, Green, Red, const Channel*> BgrConstRef;

typedef Color4RefTmpl<Red, Green, Blue, Alpha, Channel*> RgbaRef;
typedef Color4RefTmpl<Alpha, Red, Green, Blue, Channel*> ArgbRef;
typedef Color4RefTmpl<Blue, Green, Red, Alpha, Channel*> BgraRef;
typedef Color4RefTmpl<Alpha, Blue, Green, Red, Channel*> AbgrRef;

typedef Color4RefTmpl<Red, Green, Blue, Alpha, const Channel*> RgbaConstRef;
typedef Color4RefTmpl<Alpha, Red, Green, Blue, const Channel*> ArgbConstRef;
typedef Color4RefTmpl<Blue, Green, Red, Alpha, const Channel*> BgraConstRef;
typedef Color4RefTmpl<Alpha, Blue, Green, Red, const Channel*> AbgrConstRef;



//typedef GrayTmpl<Channel[1]> Gray;
//typedef GrayTmpl<Channel*>  GrayRef;
}
