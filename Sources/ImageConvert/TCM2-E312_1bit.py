"""
Copyright (c) 2018, MpicoSys
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""


__copyright__ = "Copyright (C) MpicoSys-Embedded Pico Systems"
__author__ = "Andrzej Majchrzak <andrzej.majchrzak@mpicosys.com>"
__version__ = "1.0"

from PIL import Image
import cStringIO
import sys, getopt
import os
import os.path

class TCM2312:
    """
    Class used to convert images TCM2 
    """
    __tc__ = "TCM2-E312-220"
    filename_prefix = "E312_"
    filename_sufix = "_1bit"

    def __init__(self):
        self.imagedata = None

    def get_epd_header(self):
        # EPD file format
        return b'\x3F\x05\xA0\x0A\x00\x01\x07'+('\x00'*9)

    def __convert(self, img):
        (img_width, img_height) = img.size
        assert img_width == 1440
        assert img_height == 640*4

        img=img.convert('P')
        img=img.transpose(Image.FLIP_LEFT_RIGHT)
        imgbytes = list(img.tobytes())
        imgnewbytes=list(len(imgbytes)*b'\x00')

        trans1=( # new, original
            (6,7),
            (4,6),
            (2,5),
            (0,4),
            (14,3),
            (12,2),
            (10,1),
            (8,0),
            (7,15),
            (5,14),
            (3,13),
            (1,12),
            (15,11),
            (13,10),
            (11,9),
            (9,8)
           )

        for i in range(len(imgbytes)/16):
            for t,f in trans1:
                imgnewbytes[16*(i)+t]=imgbytes[16*(i)+f]

        imgnew = Image.frombytes('P',img.size,''.join(imgnewbytes))
        palette = []
        for i in range(256):
            p=0xFF
            if i>0:
                p=0x00
            palette.extend((p, p, p))
        assert len(palette) == 768
        imgnew.putpalette(palette)
        newstr=imgnew.convert('1').tobytes()

        return newstr

    def load_image_from_string(self, str):
        # save string as IO
        src = cStringIO.StringIO()
        src.write(str)
        src.seek(0)

        img = Image.open(src)
        self.imagedata = self.__convert(img)

    def load_image(self, src):
        # Open file
        img = Image.open(src)

        self.imagedata = self.__convert(img)

    def load_image_from_pil(self, obj):
        self.imagedata = self.__convert(obj)

"""
Convert image to EPD file.
Command line usage: TCM2-E312_1bit.py -i <inputfile> 
"""
def main(argv):
    inputfile = ''
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print 'TCM2-E312_1bit.py -i <inputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'TCM2-E312_1bit.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg

    if not inputfile:
        print 'usage: TCM2-E312_1bit.py -i <inputfile>'
        sys.exit(2)

    print 'Input file is ', inputfile

    if not os.path.isfile(inputfile):
        print "Input filename is not pointing to an existing location!"
        sys.exit(2)

    tcm = TCM2312()
    (head_fname, tail_fname) = os.path.split(inputfile)
    tail_fname = tail_fname.split('.')[0]
    outputfile = os.path.join(head_fname, tcm.filename_prefix
                              +tail_fname+tcm.filename_sufix+".epd")
    
    print 'Output file is ', outputfile, ' + .dlz'
    
    tcm.load_image(inputfile)

    fil = open(outputfile, 'w+b')
    fil.seek(0)
    fil.write(tcm.get_epd_header())
    fil.write(tcm.imagedata)
    fil.close()

    os.system("bcl_tcm.exe c lz "+outputfile+" "+outputfile[:-4]+".dlz")
    #now remove 12bytes long BCL header
    f = open(outputfile[:-4]+".dlz", 'rb')
    compressed = f.read()
    f.close()
    f = open(outputfile[:-4]+".dlz", 'wb')
    f.write(compressed[12:])
    f.close()

if __name__ == "__main__":
   main(sys.argv[1:])
