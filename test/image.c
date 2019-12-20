void file_format () {

    if(Open()==true)
    {
        unsigned char testread[5];

        if(Read(&testread,(unsigned long)4)==4)
        {
            testread[4]=0;
            if(!strcmp((char *)testread,"GIF8"))
            {
                Close();
                LoadGIFImage(justsize);
            }
            else if(testread[0]==0xff && testread[1]==0xd8)
            {
                Close();
                LoadJPGImage(justsize);
            }
            else if(testread[0]==0x89 && testread[1]==0x50 && testread[2]==0x4e && testread[3]==0x47)
            {
                Close();
                LoadPNGImage(justsize);
            }
            else if(testread[0]==0x00 && testread[1]==0x00 && testread[2]==0x01 && testread[3]==0x00)
            {
                Close();
                LoadWINICOImage(justsize);
            }
            else if(testread[0]==0x42 && testread[1]==0x4d)
            {
                Close();
                LoadBMPImage(justsize);
            }
        }
    }

}

uint8 UTIL_isValidImage(const unsigned char h[5])
{

    //GIF8
    if (h[0] == 71 && h[1] == 73 && h[2] == 70 && h[3] == 56)
        return IMAGE_GIF;

    //89 PNG
    if (h[0] == 137 && h[1] == 80 && h[2] == 78 && h[3] == 71)
        return IMAGE_PNG;

    //FFD8
    if (h[0] == 255 && h[1] == 216)
        return IMAGE_JPG;

    return IMAGE_VOID;
}

// also use sdl methods using sdl_rwops