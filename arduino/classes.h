class ColourRGB {
    String name;
    int red;
    int green;
    int blue;
public:
    ColourRGB(const String& name = "No Name", int red = 255, int green = 0, int blue = 0) {
        this->name = name;
        this->red = red;
        this->green = green;
        this->blue = blue;
    }
    int     getRed() { return red; }
    int     getGreen() { return green; }
    int     getBlue() { return blue; }
    String getName() { return name; }
};
