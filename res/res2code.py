import argparse, binascii, os

header_template = '''
/************************************************************************/
/*  This file is generated automatically by res2code utility. Do not    */
/*  change it manually.                                                 */
/************************************************************************/
// Original filename: {0}

#pragma once

size_t Get_{1}_Size();
const void* Get_{1}_Buffer();

namespace tools {{
class ByteArray;
}} // namespace tools

tools::ByteArray Get_{1}_Array();

'''

source_template = '''
/************************************************************************/
/*  This file is generated automatically by res2code utility. Do not    */
/*  change it manually.                                                 */
/************************************************************************/
// Original filename: {0}

#include "byteArray.h"

namespace {{
static const unsigned char data_{1}[] = {{
{2}
}};
}} // namespace

size_t Get_{1}_Size() {{
  return sizeof(data_{1});
}}

const void* Get_{1}_Buffer() {{
  return data_{1};
}}

tools::ByteArray Get_{1}_Array() {{
  return tools::ByteArray(Get_{1}_Buffer(), Get_{1}_Size());
}}

'''

def formSrc(input_file_name):
  f = open(input_file_name, "rb")
  src_str = "  ";
  try:
    byte = f.read(1)
    add_comma = False
    line_count = 0
    while byte:
      # Do stuff with byte.
      if add_comma:
        src_str += ","

      if line_count > 12:
        src_str += "\n  "
        line_count = 0

      add_comma = True
      line_count = line_count + 1

      src_str += "0x"
      src_str += binascii.b2a_hex(byte)
      #src_str += ""

      byte = f.read(1)
  finally:
    f.close()
  return src_str

def formFuncName(file_name):
  up = True;
  result = ""
  for ch in file_name:
    if ch == '-' or ch == '.' or ch == '_':
      result += '_';
      up = True
    elif up:
      result += ch.upper()
      up = False
    else:
      result += ch
  return result


def formOutputName(input_file_name):
  result = ""
  for ch in input_file_name:
    if ch == '-' or ch == '.' or ch == '_':
      result += '_';
    else:
      result += ch
  return result.lower()

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-i", "--input", dest="input", help="Input binary file", metavar="FILE")
  parser.add_argument("-o", "--output", dest="output", help="Output source file (without extension)", metavar="FILE")
  parser.add_argument("-d", "--output_directory", dest="output_directory", help="Directory for generated files", metavar="FILE")
  args = parser.parse_args()

  head, input_filename = os.path.split(args.input)

  if not args.output:
    args.output = formOutputName(input_filename); 

  if args.output_directory:
    args.output = os.path.join(args.output_directory, args.output)

  try:
    os.makedirs(os.path.dirname(args.output))
  except os.error, e:
    pass

  output_h = args.output + ".h"
  output_cpp = args.output + ".cpp"

  do_generate = False
  input_timestamp = os.path.getmtime(input_filename)
  try:
    output_timestamp_h = os.path.getmtime(output_h)
    output_timestamp_cpp = os.path.getmtime(output_cpp)

    do_generate = input_timestamp > output_timestamp_h or input_timestamp > output_timestamp_cpp
  except os.error, e:
    do_generate = True

  if do_generate:
    func_name = formFuncName(input_filename);
    src_text = formSrc(args.input);

    out_hdr = open(output_h, "w")
    out_hdr.write(header_template.format(input_filename, func_name))

    out_src = open(output_cpp, "w")
    out_src.write(source_template.format(input_filename, func_name, src_text))
  #print src_str
