import argparse
import datetime

parser = argparse.ArgumentParser(description='Generate the modules C and H file templates')
parser.add_argument('-m', '--module', help='Module name. Like i2c or nexus_mgr', required=True)
parser.add_argument('-d', '--description', help='What is the module about.', required=True)
parser.add_argument('-a', '--author', help='Your name as author displayed on top of module', required=True)

args = parser.parse_args()

module_name = args.module.lower()
modules_name_upper = module_name.upper()
module_description = args.description
author = args.author
current_year = datetime.datetime.now().year
eol_char = "\n"

c_file_template = \
    "/*! \\file {module_name}.cpp{eol}"\
    "*{eol}"\
    "*  \\brief {module_description}.{eol}"\
    "*{eol}"\
    "*{eol}"\
    "*  \\author {author}{eol}"\
    "*  @copyright Copyright (c) Graham Riches {year}{eol}"\
    "*/{eol}"\
    "{eol}"\
    "{eol}"\
    "/********************************** Includes *******************************************/{eol}"\
    "#include \"{module_name}.h\"{eol}"\
    "{eol}"\
    "/*********************************** Consts ********************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/************************************ Types ********************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/*********************************** Macros ********************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/******************************* Global Variables **************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/******************************** Local Variables **************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/****************************** Functions Prototype ************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/****************************** Functions Definition ***********************************/{eol}"\
    "{eol}".format(eol=eol_char, module_name=module_name, module_description=module_description, author=author, year=current_year)


h_file_template = \
    "/*! \\file {module_name}.h{eol}"\
    "*{eol}"\
    "*  \\brief {module_name} module functions and variables declarations.{eol}"\
    "*{eol}"\
    "*{eol}"\
    "*  \\author {author}{eol}"\
    "*  \\copyright Copyright (c) ATTAbotics Inc. {year}{eol}"\
    "*/{eol}"\
    "{eol}"\
    "#ifndef __{modules_name_upper}_H__{eol}"\
    "#define __{modules_name_upper}_H__{eol}"\
    "{eol}"\
    "/********************************** Includes *******************************************/{eol}"\
    "{eol}"\
    "/*********************************** Consts ********************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/************************************ Types ********************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/*********************************** Macros ********************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/******************************* Global Variables **************************************/{eol}"\
    "{eol}"\
    "{eol}"\
    "/****************************** Functions Prototype ************************************/{eol}"\
    "void {modules_name_upper}_init( void );{eol}"\
    "{eol}"\
    "{eol}"\
    "{eol}"\
    "#endif /* __{modules_name_upper}_H__ */{eol}".format(eol=eol_char, module_name=module_name, modules_name_upper=modules_name_upper, module_description=module_description, author=author, year=current_year)

c_file = open("{}.cpp".format(module_name),"w+")
c_file.write(c_file_template)
c_file.close()


h_file = open("{}.h".format(module_name),"w+")
h_file.write(h_file_template)
h_file.close()
