#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main()
{
    FILE    *fpt;
    FILE    *fpt1;
    unsigned char *image;
    unsigned char *otemplate;
    float *template;
    float *msf_bit;
    unsigned char *msf;
    unsigned char *binaryimage;
    unsigned char *tempbinaryimage;
    unsigned char *marker;
    unsigned char *thinned;
    unsigned char *invbim;
    unsigned char *invthin;
    char Header[320], header[320], gt;
    int ROWS,COLS,BYTES,rows,cols,bytes,sysrow,syscol,tp,fp,tn,fn,thres,i,detected,not_detected;
    int r,r2,c,c2,flag,a,b,k,temp,flag2,bp,ep;
    double sum,sumtemp,omin,omax,tpr,fpr;

    	/* read image */
    if ((fpt=fopen("parenthood.ppm","rb")) == NULL)
        {
        printf("Unable to open parenthood.ppm for reading\n");
        exit(0);
        }
    fscanf(fpt,"%s %d %d %d",Header,&COLS,&ROWS,&BYTES);
    if (strcmp(Header,"P5") != 0  ||  BYTES != 255)
        {
        printf("Not a greyscale 8-bit PPM image\n");
        exit(0);
        }
    image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
    Header[0]=fgetc(fpt);	/* read white-space character that separates header */
    fread(image,1,COLS*ROWS,fpt);
    fclose(fpt);

    	/* read template image */
    if ((fpt=fopen("parenthood_e_template.ppm","rb")) == NULL)
        {
        printf("Unable to open parenthood_e_template.ppm for reading\n");
        exit(0);
        }
    fscanf(fpt,"%s %d %d %d",header,&cols,&rows,&bytes);
    if (strcmp(header,"P5") != 0  ||  bytes != 255)
        {
        printf("Not a greyscale 8-bit PPM image\n");
        exit(0);
        }
    otemplate=(unsigned char *)calloc(rows*cols,sizeof(unsigned char));
    header[0]=fgetc(fpt);	/* read white-space character that separates header */
    fread(otemplate,1,cols*rows,fpt);
    fclose(fpt);

        /* Zero mean template */
    template=(float *)calloc(rows*cols,sizeof(float));
    sumtemp = 0.0;

    for (r2=0; r2<rows*cols; r2++) {
        sumtemp+=otemplate[r2];
    }

    for (r2=0; r2<rows*cols; r2++) {
        template[r2]=otemplate[r2]-(sumtemp/(rows*cols));
        }

    	/* allocate memory for msf bits version of image */
    msf_bit=(float *)calloc(ROWS*COLS,sizeof(float));

    omax = 0.0;
    omin = 9999999999.0;

    	/* msf bits version of image */
    for (r=0; r<ROWS; r++) {
        if (r<7 || r>=ROWS-7)
            continue;
        else {
            for (c=0; c<COLS; c++)
            {
                if(c<4 || c>=COLS-4)
                    continue;
                else {
                    sum=0.0;
                    for (r2=-(rows/2); r2<=(rows/2); r2++)
                        for (c2=-(cols/2); c2<=(cols/2); c2++)
                            sum+=(image[(r+r2)*COLS+(c+c2)]*template[(r2+(rows/2))*cols+(c2+(cols/2))]);
                }
            if (omax<=sum)
                omax = sum;
            if (omin>=sum)
                omin = sum;
            msf_bit[r*COLS+c]=sum;
            }
        }
    }

        /* normalising the image to obtain msf image */
    msf=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

    for (r=0; r<ROWS; r++) {
        for (c=0; c<COLS; c++)
        {
            msf[r*COLS+c] = round((msf_bit[r*COLS+c]-omin)*(255.0/(omax-omin)));
        }
    }

    printf("Creating Outputs.csv file\n");
    fpt1=fopen("Outputs.csv","w");
    fprintf(fpt1,"Threshold,TP,FP,TPR,FPR\n");

    for (thres=170; thres<=245; thres++) {
            /* Reading the ground truth data */
        if ((fpt=fopen("parenthood_gt.txt","r")) == NULL)
            {
            printf("Unable to open parenthood_gt.txt for reading\n");
            exit(0);
            }
        tp=fp=tn=fn=0;
        tpr=fpr=0.0;
        while (1)
        {
            detected=0;
            not_detected=0;
            i = fscanf(fpt,"%s %d %d",&gt,&syscol,&sysrow);
            if (i!=3)
                break;
            else {
                for (r=-(rows/2); r<=(rows/2); r++) {
                    for (c=-(cols/2); c<=(cols/2); c++) {
                        if (msf[(sysrow+r)*COLS+(syscol+c)]>thres)
                            detected=1;
                        else if (msf[(sysrow+r)*COLS+(syscol+c)]<=thres)
                            not_detected=1;
                        }
                    }

                if(detected==1) {
                    detected=not_detected=0;
                    binaryimage = (unsigned char *)calloc(rows*cols,sizeof(unsigned char));
                    tempbinaryimage=(unsigned char *)calloc((rows+2)*(cols+2),sizeof(unsigned char));
                    thinned=(unsigned char *)calloc(rows*cols,sizeof(unsigned char));
                    invbim = (unsigned char *)calloc(rows*cols,sizeof(unsigned char));
                    invthin=(unsigned char *)calloc(rows*cols,sizeof(unsigned char));
                    flag=0;
                    for (r=-(rows/2); r<=(rows/2); r++) {
                        for (c=-(cols/2); c<=(cols/2); c++) {
                            if (image[(sysrow+r)*COLS+(syscol+c)]>128) {
                                binaryimage[flag] = 0;
                                invbim[flag] = 255;}
                            else if(image[(sysrow+r)*COLS+(syscol+c)]<=128) {
                                binaryimage[flag] = 255;
                                invbim[flag] = 0;}
                            flag++;
                        }
                    }
                    for(r=0; r<rows; r++) {
                        for(c=0; c<cols; c++) {
                            temp = binaryimage[r*cols+c];
                            tempbinaryimage[(r+1)*(cols+2)+(c+1)] = temp;
                        }
                    }
                    while (2) {
                        marker=(unsigned char *)calloc((rows+2)*(cols+2),sizeof(unsigned char));
                        for(r=1; r<=rows; r++) {
                            for(c=1; c<=cols; c++) {
                                if(tempbinaryimage[r*(cols+2)+c]==255) {
                                    a=0; r2=-1;
                                    for(c2=-1; c2<1; c2++) {
                                        if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+r2)*(cols+2)+(c+(c2+1))]==0)
                                            a=a+1;
                                    }
                                    c2=1;
                                    for(r2=-1; r2<1; r2++) {
                                        if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+(r2+1))*(cols+2)+(c+c2)]==0)
                                            a=a+1;
                                    }
                                    r2=1;
                                    for(c2=1; c2>-1; c2--) {
                                        if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+r2)*(cols+2)+(c+(c2-1))]==0)
                                            a=a+1;
                                    }
                                    c2=-1;
                                    for(r2=1; r2>-1; r2--) {
                                        if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+(r2-1))*(cols+2)+(c+c2)]==0)
                                            a=a+1;
                                    }

                                    b=0; k=0;
                                    for(r2=-1; r2<=1; r2++) {
                                        for(c2=-1; c2<=1; c2++) {
                                            if(r2!=0 || c2!=0){
                                                if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255) {
                                                    b=b+1;
                                                }
                                            }
                                        }
                                    }
                                    if(tempbinaryimage[(r-1)*(cols+2)+c]==0 || tempbinaryimage[r*(cols+2)+(c+1)]==0 || (tempbinaryimage[(r+1)*(cols+2)+c]==0 && tempbinaryimage[r*(cols+2)+(c-1)]==0))
                                        k=1;

                                    if(a==1 && b>=2 && b<=6 && k==1) {
                                        marker[r*(cols+2)+c]=255;
                                    }
                                }
                                else if(tempbinaryimage[r*(cols+2)+c]==0)
                                    continue;
                            }
                        }
                        flag2=0;
                        for(r2=1; r2<=rows; r2++) {
                            for(c2=1; c2<=cols; c2++) {
                                if(marker[r2*(cols+2)+c2]==255) {
                                    tempbinaryimage[r2*(cols+2)+c2]=0;
                                    flag2=flag2+1;
                                }
                            }
                        }
                        if(flag2==0)
                            break;
                    }
                    for(r=0; r<rows; r++) {
                        for(c=0; c<cols; c++) {
                            thinned[r*cols+c] = tempbinaryimage[(r+1)*(cols+2)+(c+1)];
                        }
                    }
                    for(r=0; r<rows; r++) {
                        for(c=0; c<cols; c++) {
                            if(thinned[r*cols+c]==0)
                                invthin[r*cols+c]=255;
                            else if(thinned[r*cols+c]==255)
                                invthin[r*cols+c]=0;
                        }
                    }
                    ep=0;bp=0;
                    for(r=1; r<=rows; r++) {
                        for(c=1; c<=cols; c++) {
                            if(tempbinaryimage[r*(cols+2)+c]==255) {
                                a=0; r2=-1;
                                for(c2=-1; c2<1; c2++) {
                                    if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+r2)*(cols+2)+(c+(c2+1))]==0)
                                        a=a+1;
                                }
                                c2=1;
                                for(r2=-1; r2<1; r2++) {
                                    if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+(r2+1))*(cols+2)+(c+c2)]==0)
                                        a=a+1;
                                }
                                r2=1;
                                for(c2=1; c2>-1; c2--) {
                                    if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+r2)*(cols+2)+(c+(c2-1))]==0)
                                        a=a+1;
                                }
                                c2=-1;
                                for(r2=1; r2>-1; r2--) {
                                    if(tempbinaryimage[(r+r2)*(cols+2)+(c+c2)]==255 && tempbinaryimage[(r+(r2-1))*(cols+2)+(c+c2)]==0)
                                        a=a+1;
                                }
                                if(a==1)
                                    ep=ep+1;
                                else if(a>2)
                                    bp=bp+1;
                            }
                        }
                    }
                    if(ep==1 && bp==1)
                        detected = 1;
                    else if(ep!=1 || bp!=1)
                        not_detected = 1;
                }
                if (gt=='e' && detected==1)
                    tp=tp+1;
                else if (gt!='e' && detected==1)
                    fp=fp+1;
                else if (gt!='e' && not_detected==1)
                    tn=tn+1;
                else if (gt=='e' && not_detected==1)
                    fn=fn+1;
            }
        }
        fclose(fpt);

        tpr = (double)(tp)/(tp+fn);
        fpr = (double)(fp)/(fp+tn);

        /* Outputting TP, FP, TPR and FPR for each Threshold (T) in Outputs.csv */
        fprintf(fpt1,"%d,%d,%d,%lf,%lf\n",thres,tp,fp,tpr,fpr);
    }

    printf("Outputs.csv file created");
    fclose(fpt1);

    	/* write out MSF image to see result */
    fpt=fopen("msf_e.ppm","wb");
    fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
    fwrite(msf,COLS*ROWS,1,fpt);
    fclose(fpt);

    fpt=fopen("binaryimage.ppm","wb");
    fprintf(fpt,"P5 %d %d 255\n",cols,rows);
    fwrite(invbim,cols*rows,1,fpt);
    fclose(fpt);

    fpt=fopen("thinnedimage.ppm","wb");
    fprintf(fpt,"P5 %d %d 255\n",cols,rows);
    fwrite(invthin,cols*rows,1,fpt);
    fclose(fpt);
}
