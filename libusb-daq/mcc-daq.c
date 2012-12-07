#include <libusb-1.0/libusb.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VENDOR_MCC          0x09DB // Required
#define PRODUCT_MCC_USB2408 0x00FE // Required

#define OUTBUF_MAX  1024

#define USB_ENDPOINT_OUT 0x01 // Can be queried from device
#define USB_ENDPOINT_IN 0x81  // Can be queried from device
#define USB_TIMEOUT 500

int gExpectedSamples;
int gExpectedChannels;

// Dump bulk data to stderr as gathered
void dumpBulkData(const double *buffer, int samples, int channels)
{
    int i, j;
    
    for (i = 0; i < samples; i++) {
        for (j = 0; j < channels; j++) {
            printf("%f", buffer[i*channels + j]);
            if (j < channels - 1)
                printf("\t");
        }
        printf("\n");
    }
}

// Thread for bulk input testing -- should deliver data to stderr when the
// AISCAN command is given to the DAQ device.
void bulkIn(void *arg)
{
    char buf[8001];
    libusb_device_handle *daqHandle = (libusb_device_handle*)arg;
    int b, err;
    memset(buf, 0, 8000);
  
    while(1) { 
        err = libusb_bulk_transfer(daqHandle, 
                                   USB_ENDPOINT_IN, 
                                   (unsigned char*)buf, 
                                   sizeof(buf)-1, 
                                   &b, 
                                   100);
    
        if (!err) {
            buf[b] = 0;

            fprintf(stderr, "%d bytes.\n", b);
            dumpBulkData((double*)buf, gExpectedSamples, gExpectedChannels);
        }
    }
}

size_t strControlTransferOut(libusb_device_handle *daqHandle, const char *s) {

    unsigned char buf[OUTBUF_MAX];
    size_t bufLength = strlen(s);
    size_t bytesOut;
    int b;
    
    bytesOut = 0;
    while(bufLength) {
        if (bufLength > OUTBUF_MAX)
            b = OUTBUF_MAX;
        else b = bufLength;
   
        memcpy((char*)buf, s, bufLength);
        s += bufLength; 
 
        b = libusb_control_transfer(daqHandle,
                        0x40,
                        0x80, 
                        0, 
                        0, 
                        (unsigned char*)buf, b, 10); 
        if (b > 0) {
            bytesOut += b;
            bufLength -= b;
        } 
        else { // Handle error
            fprintf(stderr, "Error in control out: %d.\n", b);
            break;
        }
    }
    return bytesOut; 
}

void daqTest(libusb_device *daqDevice)
{
    int err;
    char buf[1024];
    char epDesc[64];
    libusb_endpoint_descriptor *e;
    char *d;
    int epInAddr, epOutAddr;
    int b, n;
    pthread_t t1;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    
    epInAddr = epOutAddr = 0;

    printf("Starting daq test.\n");    
    int s = libusb_get_device_speed(daqDevice);
    printf("Speed: %d\n", s);
    int c;
    libusb_device_handle *daqHandle;
    err = libusb_open(daqDevice, &daqHandle);
    err = libusb_claim_interface(daqHandle, 0);

    err = libusb_get_configuration(daqHandle, &c);
    printf ("Config: %d\n", s);
   
    // Get descriptors
    err = libusb_control_transfer(daqHandle, 
                        0x80, 
                        LIBUSB_REQUEST_GET_DESCRIPTOR, 
                        (0x02 << 8) | 0, 0, 
                        (unsigned char*)epDesc, sizeof(epDesc), 1000);
  
    printf("Control transfer bytes transferred: %d.\n", err);
    
    d = &epDesc[0];
    while (d[1] != 0x05) 
        d += d[0];
    
    e = (libusb_endpoint_descriptor*)d;
    int l = e->bLength;
    if(e->bDescriptorType == 0x05) {
        if (e->bEndpointAddress & 0x80) epInAddr = e->bEndpointAddress;
        else epOutAddr = e->bEndpointAddress;
    }
    e = (libusb_endpoint_descriptor*)(d + l);
    if(e->bDescriptorType == 0x05) {
        if (e->bEndpointAddress & 0x80) epInAddr = e->bEndpointAddress;
        else epOutAddr = e->bEndpointAddress;
    } 
    printf("eOut: %d, eIn: %d\n", epOutAddr, epInAddr);

    b = strControlTransferOut(daqHandle, "AISCAN:LOWCHAN=0");
    b = strControlTransferOut(daqHandle, "AISCAN:HIGHCHAN=1");
    b = strControlTransferOut(daqHandle, "AISCAN:RANGE=BIP10V");
    //b = strControlTransferOut(daqHandle, "AISCAN:CAL=ENABLE");
    b = strControlTransferOut(daqHandle, "AISCAN:RATE=100.00");
    if (b > 0)
        printf("%d bytes sent.\n", err);
    else printf("Err: %d\n", err); 
  
    // Get serial number
    b = strControlTransferOut(daqHandle, "?DEV:MFGSER"); 
    b = libusb_control_transfer(daqHandle,
                        0xC0,
                        0x80,
                        0, 
                        0, 
                        (unsigned char*)buf, sizeof(buf), 1000);
    if (b > 0) {
        buf[b] = 0;
        printf("%s\n",buf);
    } else printf("Err: %d\n", b);
 
    // Attempt to get max rate
    // EDIT: Turns out this is a reflection command, disregard.
    /*
    b = strControlTransferOut(daqHandle, "?AI:MAXSCANRATE"); 
    b = libusb_control_transfer(daqHandle,
                        0xC0,
                        0x80,
                        0, 
                        0, 
                        (unsigned char*)buf, sizeof(buf), 1000);
    if (b > 0) {
        buf[b] = 0;
        printf("%s\n",buf);
    } else printf("Err: %d\n", b);
    */
    printf("Requesting bulk data...\n");
    fflush(stdout);

    gExpectedSamples = 500;
    gExpectedChannels = 2;
    b = strControlTransferOut(daqHandle, "AISCAN:SAMPLES=500");
    b = strControlTransferOut(daqHandle, "AISCAN:START");  
        printf("Starting bulk input collection.\n");
    pthread_create(&t1, &attr, (void*(*)(void*))bulkIn, daqHandle);


    fprintf(stderr, "Waiting...\n");
    sleep(3);
    b = strControlTransferOut(daqHandle, "AISCAN:STOP");
    sleep(6);

    printf("Shutting down...\n");
    libusb_release_interface(daqHandle, 0);
    libusb_close(daqHandle);
}

void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0;

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}

		printf("%04x:%04x (bus %d, device %d)\n",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));
        
        if (desc.idVendor == VENDOR_MCC && 
            desc.idProduct == PRODUCT_MCC_USB2408)
                daqTest(dev);

	}
}

int main(int argc, char **argv)
{
 	libusb_device **devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return (int) cnt;

	print_devs(devs);
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return 0;
}
