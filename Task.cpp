#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Task.h"
#include "ThreadPool.h"

using namespace std;
enum allocation_type{
    NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

typedef  struct {
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    enum allocation_type allocation_;
}Image;


void Image_load(Image *img,const char *fname);
void Image_create(Image *img, int width, int height, int channels,bool zeroed);
void Image_save(const Image *img, const char *fname);
void Image_free(Image *img);
void do_something(const Image *orig, Image *conv);


void Image_load(Image *img, const char *fname) {

    if((img->data= stbi_load(fname, &img->width, &img->height, &img->channels, 0)) != NULL){
        img->size = img->width * img->height * img->channels;
        img->allocation_=STB_ALLOCATED;
    }
}

void Image_create(Image *img, int width, int height, int channels,bool zeroed){
    size_t size = width * height * channels;
    if(zeroed){
        img->data = static_cast<uint8_t *>(calloc(size, 1));
    }
    else{
        img->data = static_cast<uint8_t *>(malloc(size));
    }
    if(img->data!=NULL){
        img->width = width;
        img->height = height;
        img->size = size;
        img->channels = channels;
        img->allocation_ = SELF_ALLOCATED;
    }
}
void Image_save(const Image *img, const char *fname){
    stbi_write_png(fname, img->width,img->height,img->channels, img->data, img->width*img->channels);
}

void Image_free(Image *img) {
    if(img->allocation_ != NO_ALLOCATION && img->data != NULL){
        if(img->allocation_ == STB_ALLOCATED) { stbi_image_free(img->data);
        } else {
            free(img->data);
        }
        img->data = NULL;
        img->width = 0;
        img->height = 0;
        img->size = 0;
        img->allocation_ = NO_ALLOCATION;
    }
}

void do_something(const Image *orig, Image *conv){
    Image_create(conv, orig->width, orig->height, 1, false);
    for(unsigned char *p = orig->data, *pg = conv->data; p != orig->data+orig->size; p += orig->channels, pg += conv->channels){
        *pg = (uint8_t)((*p)*0.299 + *(p+1)*0.587+ *(p+2)*0.144);
    }
}


Task::Task(char *img, int index) : _img(img), _index(index){}

thread Task::run(){
    char path[] = "images\\";
    strcat(path, _img);
    Image old_image;
    Image_load(&old_image, path);
    Image conv_image;
    //convert the img
    do_something(&old_image, &conv_image);
    //save the converted img as new png file
    char filename[12];
    char idx[2];
    strcpy(filename,"image_");
    snprintf(idx,2, "%d", _index);
    strcat(filename, idx);
    strcat(filename,".png");
    Image_save(&conv_image, filename);
    //free space
    Image_free(&old_image);
    Image_free(&conv_image);
    cout << _index << ":image saved\n";
}

bool Task::last_one() {
    return (_index == 10);
}

Task::~Task() = default;