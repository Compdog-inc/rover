from pathlib import Path
import glob

print("")
print("==========================================")
print("       Running header AutoGen task        ")
print("==========================================")
print("")

conf_defines = {}

if Path.exists(Path("autogen.conf")):
    f = open("autogen.conf", "r")
    lines = f.readlines()
    f.close()
    for line in lines:
        parts = line.split("=",1)
        conf_defines[parts[0]] = parts[1]

if conf_defines.get("VERSION_32") == None:
    conf_defines["VERSION_32"] = 1

def reformatText(text):
    for name in conf_defines:
        text = text.replace("$"+name+"$", str(conf_defines[name]))
    return text


def generateFile(path):
    pt = Path(path)

    print("Parsing " + pt.name)
    f = open(path, "r")
    text = f.read()
    f.close()

    text = reformatText(text)

    output = pt.with_suffix("").with_suffix(".h")
    print("Generating "+output.name)
    f = open(output.resolve(), "w")
    f.write(text)
    f.close()
    print("Finished writing to "+output.name)


files = glob.glob('include/*.gen.h')
for file in files:
    generateFile(file)

# Update config
conf_defines["VERSION_32"] = str(int(conf_defines["VERSION_32"]) + 1)
    
print("Saving new configuration file")
lines = []
for name in conf_defines:
    lines.append(name+"="+str(conf_defines[name]))
f = open("autogen.conf", "w")
f.writelines(lines)
f.close()

print("")
print("==========================================")
print("   Successfully generated header files.   ")
print("==========================================")
print("")