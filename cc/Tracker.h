
struct Image_format{

};

class Tracker{
public:
	virtual void process(uint8_t img, Image_format format);
};

class Tracking_point{

};

class Tracker_v1:public Tracker{

	virtual void process(uint8_t img, Image_format format);
};

void Tracker_v1::process(uint8_t img, Image_format format){

}
