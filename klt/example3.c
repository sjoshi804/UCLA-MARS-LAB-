/**********************************************************************
Finds the 150 best features in an image and tracks them through the 
next two images.  The sequential mode is set in order to speed
processing.  The features are stored in a feature table, which is then
saved to a text file; each feature list is also written to a PPM file.
**********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pnmio.h"
#include "klt.h"

#define REPLACE 

#ifdef WIN32
int RunExample3()
#else
int main()
#endif
{
  unsigned char *img1, *img2;
  char fnamein[100], fnameout[100];
  KLT_TrackingContext tc;
  KLT_FeatureList fl;
  KLT_FeatureTable ft;
  
  int nFeatures = 150, nFrames = 12;
  int ncols, nrows;
  int i;

  tc = KLTCreateTrackingContext();
  fl = KLTCreateFeatureList(nFeatures);
  ft = KLTCreateFeatureTable(nFrames, nFeatures);
  tc->affineConsistencyCheck = 2; 
  tc->sequentialMode = TRUE;
  tc->writeInternalImages = FALSE;
  tc->affineConsistencyCheck = 2;  /* set this to 2 to turn on affine consistency check */
  KLT_FaceList faces = KLTCreateFaceList(2);
  VJ_Face face1 = VJCreateFace(220, 92, 36, 36);
  VJ_Face face2 = VJCreateFace(76, 83, 43, 43);
  faces.faceList[0] = face1;
  faces.faceList[1] = face2;
  img1 = pgmReadFile("demo1.pgm", NULL, &ncols, &nrows);
  img2 = (unsigned char *) malloc(ncols*nrows*sizeof(unsigned char));

  KLTSelectGoodFeatures(tc, img1, ncols, nrows, fl, &faces);
  KLTStoreFeatureList(fl, ft, 0);
  KLTWriteFeatureListToPPM(fl, img1, ncols, nrows, "oframe1.ppm");

  for (i = 2 ; i < nFrames ; i++)  {
    sprintf(fnamein, "demo%d.pgm", i);
    pgmReadFile(fnamein, img2, &ncols, &nrows);
    KLTTrackFeatures(tc, img1, img2, ncols, nrows, fl);
#ifdef REPLACE
    KLTReplaceLostFeatures(tc, img2, ncols, nrows, fl, &faces);
#endif
    KLTStoreFeatureList(fl, ft, i);
    sprintf(fnameout, "oframe%d.ppm", i);
    KLTWriteFeatureListToPPM(fl, img2, ncols, nrows, fnameout);
  }
  KLTWriteFeatureTable(ft, "features.txt", "%5.1f");
  KLTWriteFeatureTable(ft, "features.ft", NULL);

  KLTFreeFeatureTable(ft);
  KLTFreeFeatureList(fl);
  KLTFreeTrackingContext(tc);
  free(img1);
  free(img2);

  return 0;
}

