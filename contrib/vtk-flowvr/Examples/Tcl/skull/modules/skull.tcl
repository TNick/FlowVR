package require vtk
package require vtkinteraction

package require vtkflowvrrender

vtkFlowVRModule module

set datadir $env(VTK_FLOWVR_DATA_ROOT)

# Create the standard renderer, render window
# and interactor
vtkRenderer ren1
vtkRenderWindow renWin
    renWin AddRenderer ren1
#vtkRenderWindowInteractor iren
#    iren SetRenderWindow renWin

# Create the reader for the data
vtkImageReader2 reader
  reader SetDataScalarTypeToUnsignedChar
  reader SetNumberOfScalarComponents 1
  reader SetFileDimensionality 3
  reader SetFileName "$datadir/CT-Head-8.pvm.raw"
  reader SetDataExtent 0 255 0 255 0 112
vtkPiecewiseFunction opacityTransferFunction
vtkColorTransferFunction colorTransferFunction
  colorTransferFunction AddRGBPoint 0.0 1 0.92997 0.821668
  opacityTransferFunction AddPoint 0.0 0
  colorTransferFunction AddRGBPoint 1.0 1 0.903012 0.227206
  opacityTransferFunction AddPoint 1.0 0
  colorTransferFunction AddRGBPoint 2.0 1 0.71086 0.301651
  opacityTransferFunction AddPoint 2.0 0
  colorTransferFunction AddRGBPoint 3.0 1 0.478912 0.30257
  opacityTransferFunction AddPoint 3.0 0
  colorTransferFunction AddRGBPoint 4.0 1 0.842454 0.20129
  opacityTransferFunction AddPoint 4.0 0
  colorTransferFunction AddRGBPoint 5.0 1 0.550323 0.898627
  opacityTransferFunction AddPoint 5.0 0
  colorTransferFunction AddRGBPoint 6.0 0.591365 1 0.032758
  opacityTransferFunction AddPoint 6.0 0
  colorTransferFunction AddRGBPoint 7.0 0.409435 1 0.4385
  opacityTransferFunction AddPoint 7.0 0
  colorTransferFunction AddRGBPoint 8.0 0.568171 1 0.968658
  opacityTransferFunction AddPoint 8.0 0
  colorTransferFunction AddRGBPoint 9.0 0.629336 1 0.839163
  opacityTransferFunction AddPoint 9.0 0
  colorTransferFunction AddRGBPoint 10.0 0.710154 1 0.977732
  opacityTransferFunction AddPoint 10.0 0
  colorTransferFunction AddRGBPoint 11.0 0.747667 1 0.907163
  opacityTransferFunction AddPoint 11.0 0
  colorTransferFunction AddRGBPoint 12.0 0.790815 1 0.698969
  opacityTransferFunction AddPoint 12.0 0
  colorTransferFunction AddRGBPoint 13.0 0.939969 1 0.569107
  opacityTransferFunction AddPoint 13.0 0
  colorTransferFunction AddRGBPoint 14.0 0.990068 1 0.364789
  opacityTransferFunction AddPoint 14.0 0
  colorTransferFunction AddRGBPoint 15.0 1 0.977533 0.220937
  opacityTransferFunction AddPoint 15.0 0
  colorTransferFunction AddRGBPoint 16.0 1 0.846213 0.127319
  opacityTransferFunction AddPoint 16.0 0
  colorTransferFunction AddRGBPoint 17.0 1 0.692191 0.043646
  opacityTransferFunction AddPoint 17.0 0
  colorTransferFunction AddRGBPoint 18.0 1 0.61453 0.00796944
  opacityTransferFunction AddPoint 18.0 0
  colorTransferFunction AddRGBPoint 19.0 1 0.656903 2.71201e-05
  opacityTransferFunction AddPoint 19.0 0
  colorTransferFunction AddRGBPoint 20.0 1 0.677297 0.00201148
  opacityTransferFunction AddPoint 20.0 0
  colorTransferFunction AddRGBPoint 21.0 1 0.671756 0.00377691
  opacityTransferFunction AddPoint 21.0 0
  colorTransferFunction AddRGBPoint 22.0 1 0.6512 0.00561696
  opacityTransferFunction AddPoint 22.0 0
  colorTransferFunction AddRGBPoint 23.0 1 0.598662 0.0111933
  opacityTransferFunction AddPoint 23.0 0
  colorTransferFunction AddRGBPoint 24.0 1 0.576708 0.0106806
  opacityTransferFunction AddPoint 24.0 0
  colorTransferFunction AddRGBPoint 25.0 1 0.538529 0.0145238
  opacityTransferFunction AddPoint 25.0 0
  colorTransferFunction AddRGBPoint 26.0 1 0.475421 0.0195845
  opacityTransferFunction AddPoint 26.0 0
  colorTransferFunction AddRGBPoint 27.0 1 0.446721 0.029373
  opacityTransferFunction AddPoint 27.0 0
  colorTransferFunction AddRGBPoint 28.0 1 0.428863 0.0529935
  opacityTransferFunction AddPoint 28.0 0
  colorTransferFunction AddRGBPoint 29.0 1 0.412392 0.0827672
  opacityTransferFunction AddPoint 29.0 0
  colorTransferFunction AddRGBPoint 30.0 1 0.403333 0.113174
  opacityTransferFunction AddPoint 30.0 0
  colorTransferFunction AddRGBPoint 31.0 1 0.483468 0.124334
  opacityTransferFunction AddPoint 31.0 0
  colorTransferFunction AddRGBPoint 32.0 1 0.562391 0.12293
  opacityTransferFunction AddPoint 32.0 0
  colorTransferFunction AddRGBPoint 33.0 1 0.624177 0.105366
  opacityTransferFunction AddPoint 33.0 0
  colorTransferFunction AddRGBPoint 34.0 1 0.644358 0.085244
  opacityTransferFunction AddPoint 34.0 0
  colorTransferFunction AddRGBPoint 35.0 1 0.676736 0.0732208
  opacityTransferFunction AddPoint 35.0 0
  colorTransferFunction AddRGBPoint 36.0 1 0.681089 0.0574334
  opacityTransferFunction AddPoint 36.0 0
  colorTransferFunction AddRGBPoint 37.0 1 0.707548 0.047969
  opacityTransferFunction AddPoint 37.0 0
  colorTransferFunction AddRGBPoint 38.0 1 0.752576 0.0442646
  opacityTransferFunction AddPoint 38.0 0
  colorTransferFunction AddRGBPoint 39.0 1 0.790024 0.0424256
  opacityTransferFunction AddPoint 39.0 0
  colorTransferFunction AddRGBPoint 40.0 1 0.798333 0.0453883
  opacityTransferFunction AddPoint 40.0 0
  colorTransferFunction AddRGBPoint 41.0 1 0.836402 0.0480432
  opacityTransferFunction AddPoint 41.0 0
  colorTransferFunction AddRGBPoint 42.0 1 0.885724 0.0501684
  opacityTransferFunction AddPoint 42.0 0
  colorTransferFunction AddRGBPoint 43.0 1 0.866407 0.0587416
  opacityTransferFunction AddPoint 43.0 0
  colorTransferFunction AddRGBPoint 44.0 1 0.934424 0.0731888
  opacityTransferFunction AddPoint 44.0 0
  colorTransferFunction AddRGBPoint 45.0 0.994228 1 0.0790825
  opacityTransferFunction AddPoint 45.0 0
  colorTransferFunction AddRGBPoint 46.0 0.94308 1 0.0846544
  opacityTransferFunction AddPoint 46.0 0
  colorTransferFunction AddRGBPoint 47.0 0.864189 1 0.0713297
  opacityTransferFunction AddPoint 47.0 0
  colorTransferFunction AddRGBPoint 48.0 0.858597 1 0.0724665
  opacityTransferFunction AddPoint 48.0 0
  colorTransferFunction AddRGBPoint 49.0 0.78166 1 0.0688915
  opacityTransferFunction AddPoint 49.0 0
  colorTransferFunction AddRGBPoint 50.0 0.784172 1 0.0759311
  opacityTransferFunction AddPoint 50.0 0
  colorTransferFunction AddRGBPoint 51.0 0.794834 1 0.0668638
  opacityTransferFunction AddPoint 51.0 0
  colorTransferFunction AddRGBPoint 52.0 0.751806 1 0.0679342
  opacityTransferFunction AddPoint 52.0 0
  colorTransferFunction AddRGBPoint 53.0 0.684048 1 0.0649387
  opacityTransferFunction AddPoint 53.0 0
  colorTransferFunction AddRGBPoint 54.0 0.702482 1 0.0592681
  opacityTransferFunction AddPoint 54.0 0
  colorTransferFunction AddRGBPoint 55.0 0.726713 1 0.0503922
  opacityTransferFunction AddPoint 55.0 0
  colorTransferFunction AddRGBPoint 56.0 0.762199 1 0.0577818
  opacityTransferFunction AddPoint 56.0 0
  colorTransferFunction AddRGBPoint 57.0 0.782394 1 0.03118
  opacityTransferFunction AddPoint 57.0 0
  colorTransferFunction AddRGBPoint 58.0 0.825766 1 0.0340061
  opacityTransferFunction AddPoint 58.0 0
  colorTransferFunction AddRGBPoint 59.0 0.888685 1 0.02558
  opacityTransferFunction AddPoint 59.0 0
  colorTransferFunction AddRGBPoint 60.0 0.943596 1 0.0286949
  opacityTransferFunction AddPoint 60.0 0
  colorTransferFunction AddRGBPoint 61.0 0.945358 1 0.0272292
  opacityTransferFunction AddPoint 61.0 0
  colorTransferFunction AddRGBPoint 62.0 1 0.988697 0.030338
  opacityTransferFunction AddPoint 62.0 0
  colorTransferFunction AddRGBPoint 63.0 1 0.928585 0.0229579
  opacityTransferFunction AddPoint 63.0 0
  colorTransferFunction AddRGBPoint 64.0 1 0.910575 0.0214532
  opacityTransferFunction AddPoint 64.0 0
  colorTransferFunction AddRGBPoint 65.0 1 0.910804 0.0237857
  opacityTransferFunction AddPoint 65.0 0
  colorTransferFunction AddRGBPoint 66.0 1 0.861488 0.0186604
  opacityTransferFunction AddPoint 66.0 0
  colorTransferFunction AddRGBPoint 67.0 1 0.855696 0.0060358
  opacityTransferFunction AddPoint 67.0 0
  colorTransferFunction AddRGBPoint 68.0 1 0.894566 0.00979364
  opacityTransferFunction AddPoint 68.0 0
  colorTransferFunction AddRGBPoint 69.0 1 0.833587 0.00324631
  opacityTransferFunction AddPoint 69.0 0
  colorTransferFunction AddRGBPoint 70.0 1 0.84007 0.00726467
  opacityTransferFunction AddPoint 70.0 0
  colorTransferFunction AddRGBPoint 71.0 1 0.794326 0.0081324
  opacityTransferFunction AddPoint 71.0 0
  colorTransferFunction AddRGBPoint 72.0 1 0.756528 0.00806719
  opacityTransferFunction AddPoint 72.0 0
  colorTransferFunction AddRGBPoint 73.0 1 0.71007 0.0241543
  opacityTransferFunction AddPoint 73.0 0
  colorTransferFunction AddRGBPoint 74.0 1 0.718118 0.0156811
  opacityTransferFunction AddPoint 74.0 0
  colorTransferFunction AddRGBPoint 75.0 1 0.770153 0.0109777
  opacityTransferFunction AddPoint 75.0 0
  colorTransferFunction AddRGBPoint 76.0 1 0.831494 0.00311154
  opacityTransferFunction AddPoint 76.0 0
  colorTransferFunction AddRGBPoint 77.0 1 0.886639 0.000579774
  opacityTransferFunction AddPoint 77.0 0
  colorTransferFunction AddRGBPoint 78.0 1 0.911412 1.62125e-05
  opacityTransferFunction AddPoint 78.0 0
  colorTransferFunction AddRGBPoint 79.0 1 0.95734 2.6226e-06
  opacityTransferFunction AddPoint 79.0 0
  colorTransferFunction AddRGBPoint 80.0 1 0.935123 0.00450408
  opacityTransferFunction AddPoint 80.0 0
  colorTransferFunction AddRGBPoint 81.0 1 0.955825 0.0102148
  opacityTransferFunction AddPoint 81.0 0
  colorTransferFunction AddRGBPoint 82.0 1 0.926852 0.0173899
  opacityTransferFunction AddPoint 82.0 0
  colorTransferFunction AddRGBPoint 83.0 1 0.945207 0.0382277
  opacityTransferFunction AddPoint 83.0 0
  colorTransferFunction AddRGBPoint 84.0 1 0.956674 0.0584232
  opacityTransferFunction AddPoint 84.0 0
  colorTransferFunction AddRGBPoint 85.0 1 0.96716 0.0874038
  opacityTransferFunction AddPoint 85.0 0
  colorTransferFunction AddRGBPoint 86.0 1 0.976011 0.137499
  opacityTransferFunction AddPoint 86.0 0
  colorTransferFunction AddRGBPoint 87.0 0.995686 1 0.18036
  opacityTransferFunction AddPoint 87.0 0
  colorTransferFunction AddRGBPoint 88.0 0.967105 1 0.265179
  opacityTransferFunction AddPoint 88.0 0
  colorTransferFunction AddRGBPoint 89.0 1 0.946656 0.32289
  opacityTransferFunction AddPoint 89.0 0
  colorTransferFunction AddRGBPoint 90.0 1 0.890819 0.438243
  opacityTransferFunction AddPoint 90.0 0
  colorTransferFunction AddRGBPoint 91.0 1 0.667371 0.68383
  opacityTransferFunction AddPoint 91.0 0
  colorTransferFunction AddRGBPoint 92.0 1 0.603626 1
  opacityTransferFunction AddPoint 92.0 0
  colorTransferFunction AddRGBPoint 93.0 0.895447 0.503244 1
  opacityTransferFunction AddPoint 93.0 0
  colorTransferFunction AddRGBPoint 94.0 0.764845 0.403168 1
  opacityTransferFunction AddPoint 94.0 0
  colorTransferFunction AddRGBPoint 95.0 0.788838 0.428579 1
  opacityTransferFunction AddPoint 95.0 0
  colorTransferFunction AddRGBPoint 96.0 0.696827 0.301928 1
  opacityTransferFunction AddPoint 96.0 0
  colorTransferFunction AddRGBPoint 97.0 0.703745 0.295639 1
  opacityTransferFunction AddPoint 97.0 0
  colorTransferFunction AddRGBPoint 98.0 0.685262 0.286378 1
  opacityTransferFunction AddPoint 98.0 0
  colorTransferFunction AddRGBPoint 99.0 0.847508 0.194778 1
  opacityTransferFunction AddPoint 99.0 0
  colorTransferFunction AddRGBPoint 100.0 1 0.110966 0.689653
  opacityTransferFunction AddPoint 100.0 0
  colorTransferFunction AddRGBPoint 101.0 1 0.122252 0.393581
  opacityTransferFunction AddPoint 101.0 0
  colorTransferFunction AddRGBPoint 102.0 1 0.108288 0.251925
  opacityTransferFunction AddPoint 102.0 0
  colorTransferFunction AddRGBPoint 103.0 1 0.0808933 0.425968
  opacityTransferFunction AddPoint 103.0 0
  colorTransferFunction AddRGBPoint 104.0 1 0.0379704 0.667401
  opacityTransferFunction AddPoint 104.0 0
  colorTransferFunction AddRGBPoint 105.0 0.962523 0.0196431 1
  opacityTransferFunction AddPoint 105.0 0
  colorTransferFunction AddRGBPoint 106.0 0.78471 0.00901943 1
  opacityTransferFunction AddPoint 106.0 0
  colorTransferFunction AddRGBPoint 107.0 0.70894 0.00871706 1
  opacityTransferFunction AddPoint 107.0 0
  colorTransferFunction AddRGBPoint 108.0 0.655958 0.00527668 1
  opacityTransferFunction AddPoint 108.0 0
  colorTransferFunction AddRGBPoint 109.0 0.642509 0.00980633 1
  opacityTransferFunction AddPoint 109.0 0
  colorTransferFunction AddRGBPoint 110.0 0.611377 0.0133901 1
  opacityTransferFunction AddPoint 110.0 0
  colorTransferFunction AddRGBPoint 111.0 0.574439 0.0174554 1
  opacityTransferFunction AddPoint 111.0 0
  colorTransferFunction AddRGBPoint 112.0 0.51842 0.0414537 1
  opacityTransferFunction AddPoint 112.0 0
  colorTransferFunction AddRGBPoint 113.0 0.481312 0.072344 1
  opacityTransferFunction AddPoint 113.0 0
  colorTransferFunction AddRGBPoint 114.0 0.370603 0.168992 1
  opacityTransferFunction AddPoint 114.0 0
  colorTransferFunction AddRGBPoint 115.0 0.22837 0.481377 1
  opacityTransferFunction AddPoint 115.0 0
  colorTransferFunction AddRGBPoint 116.0 0.219474 0.827489 1
  opacityTransferFunction AddPoint 116.0 0
  colorTransferFunction AddRGBPoint 117.0 0.254976 1 0.99386
  opacityTransferFunction AddPoint 117.0 0
  colorTransferFunction AddRGBPoint 118.0 0.301454 1 0.853009
  opacityTransferFunction AddPoint 118.0 0
  colorTransferFunction AddRGBPoint 119.0 0.357114 1 0.627579
  opacityTransferFunction AddPoint 119.0 0
  colorTransferFunction AddRGBPoint 120.0 0.407627 1 0.363713
  opacityTransferFunction AddPoint 120.0 0
  colorTransferFunction AddRGBPoint 121.0 0.502954 1 0.279014
  opacityTransferFunction AddPoint 121.0 0
  colorTransferFunction AddRGBPoint 122.0 0.60025 1 0.281546
  opacityTransferFunction AddPoint 122.0 0
  colorTransferFunction AddRGBPoint 123.0 0.838196 1 0.54976
  opacityTransferFunction AddPoint 123.0 0
  colorTransferFunction AddRGBPoint 124.0 0.811541 0.493671 1
  opacityTransferFunction AddPoint 124.0 0
  colorTransferFunction AddRGBPoint 125.0 0.604538 0.435895 1
  opacityTransferFunction AddPoint 125.0 0
  colorTransferFunction AddRGBPoint 126.0 0.473584 0.443517 1
  opacityTransferFunction AddPoint 126.0 0
  colorTransferFunction AddRGBPoint 127.0 0.46568 0.412576 1
  opacityTransferFunction AddPoint 127.0 0
  colorTransferFunction AddRGBPoint 128.0 0.438801 0.394518 1
  opacityTransferFunction AddPoint 128.0 0
  colorTransferFunction AddRGBPoint 129.0 0.493396 0.336931 1
  opacityTransferFunction AddPoint 129.0 0
  colorTransferFunction AddRGBPoint 130.0 0.496957 0.293222 1
  opacityTransferFunction AddPoint 130.0 0
  colorTransferFunction AddRGBPoint 131.0 0.502071 0.291714 1
  opacityTransferFunction AddPoint 131.0 0
  colorTransferFunction AddRGBPoint 132.0 0.425833 0.306598 1
  opacityTransferFunction AddPoint 132.0 0
  colorTransferFunction AddRGBPoint 133.0 0.434754 0.303239 1
  opacityTransferFunction AddPoint 133.0 0
  colorTransferFunction AddRGBPoint 134.0 0.465499 0.287844 1
  opacityTransferFunction AddPoint 134.0 0
  colorTransferFunction AddRGBPoint 135.0 0.495513 0.304079 1
  opacityTransferFunction AddPoint 135.0 0
  colorTransferFunction AddRGBPoint 136.0 0.445182 0.292682 1
  opacityTransferFunction AddPoint 136.0 0
  colorTransferFunction AddRGBPoint 137.0 0.474892 0.300304 1
  opacityTransferFunction AddPoint 137.0 0
  colorTransferFunction AddRGBPoint 138.0 0.501036 0.274868 1
  opacityTransferFunction AddPoint 138.0 0
  colorTransferFunction AddRGBPoint 139.0 0.501762 0.255773 1
  opacityTransferFunction AddPoint 139.0 0.000390619
  colorTransferFunction AddRGBPoint 140.0 0.512231 0.246176 1
  opacityTransferFunction AddPoint 140.0 0.000781238
  colorTransferFunction AddRGBPoint 141.0 0.508301 0.262411 1
  opacityTransferFunction AddPoint 141.0 0.00117186
  colorTransferFunction AddRGBPoint 142.0 1 1 1
  opacityTransferFunction AddPoint 142.0 0.00156248
  colorTransferFunction AddRGBPoint 143.0 0.5503 0.23663 1
  opacityTransferFunction AddPoint 143.0 0.0019531
  colorTransferFunction AddRGBPoint 144.0 0.569529 0.24735 1
  opacityTransferFunction AddPoint 144.0 0.00234371
  colorTransferFunction AddRGBPoint 145.0 0.555138 0.224591 1
  opacityTransferFunction AddPoint 145.0 0
  colorTransferFunction AddRGBPoint 146.0 0.561645 0.243359 1
  opacityTransferFunction AddPoint 146.0 0
  colorTransferFunction AddRGBPoint 147.0 1 1 1
  opacityTransferFunction AddPoint 147.0 0
  colorTransferFunction AddRGBPoint 148.0 0.534465 0.248843 1
  opacityTransferFunction AddPoint 148.0 0
  colorTransferFunction AddRGBPoint 149.0 0.510411 0.239377 1
  opacityTransferFunction AddPoint 149.0 0
  colorTransferFunction AddRGBPoint 150.0 0.556749 0.207352 1
  opacityTransferFunction AddPoint 150.0 0
  colorTransferFunction AddRGBPoint 151.0 0.51838 0.26485 1
  opacityTransferFunction AddPoint 151.0 0
  colorTransferFunction AddRGBPoint 152.0 0.532689 0.251711 1
  opacityTransferFunction AddPoint 152.0 0
  colorTransferFunction AddRGBPoint 153.0 0.607006 0.258472 1
  opacityTransferFunction AddPoint 153.0 0.316406
  colorTransferFunction AddRGBPoint 154.0 0.529613 0.258364 1
  opacityTransferFunction AddPoint 154.0 0.296875
  colorTransferFunction AddRGBPoint 155.0 0.563943 0.272931 1
  opacityTransferFunction AddPoint 155.0 0.270833
  colorTransferFunction AddRGBPoint 156.0 0.531583 0.278454 1
  opacityTransferFunction AddPoint 156.0 0.257812
  colorTransferFunction AddRGBPoint 157.0 0.546135 0.315697 1
  opacityTransferFunction AddPoint 157.0 0.244792
  colorTransferFunction AddRGBPoint 158.0 0.498868 0.325537 1
  opacityTransferFunction AddPoint 158.0 0.238281
  colorTransferFunction AddRGBPoint 159.0 0.468469 0.322821 1
  opacityTransferFunction AddPoint 159.0 0.21875
  colorTransferFunction AddRGBPoint 160.0 0.49445 0.344296 1
  opacityTransferFunction AddPoint 160.0 0.208984
  colorTransferFunction AddRGBPoint 161.0 0.435236 0.352435 1
  opacityTransferFunction AddPoint 161.0 0.199219
  colorTransferFunction AddRGBPoint 162.0 0.422632 0.369391 1
  opacityTransferFunction AddPoint 162.0 0.189453
  colorTransferFunction AddRGBPoint 163.0 0.454654 0.344924 1
  opacityTransferFunction AddPoint 163.0 0.179687
  colorTransferFunction AddRGBPoint 164.0 0.537344 0.323565 1
  opacityTransferFunction AddPoint 164.0 0.160156
  colorTransferFunction AddRGBPoint 165.0 0.390141 0.379973 1
  opacityTransferFunction AddPoint 165.0 0.150391
  colorTransferFunction AddRGBPoint 166.0 0.408283 0.392148 1
  opacityTransferFunction AddPoint 166.0 0.140625
  colorTransferFunction AddRGBPoint 167.0 0.417779 0.393413 1
  opacityTransferFunction AddPoint 167.0 0.130859
  colorTransferFunction AddRGBPoint 168.0 0.37187 0.420888 1
  opacityTransferFunction AddPoint 168.0 0.121094
  colorTransferFunction AddRGBPoint 169.0 0.415801 0.397547 1
  opacityTransferFunction AddPoint 169.0 0.111328
  colorTransferFunction AddRGBPoint 170.0 0.406366 0.504359 1
  opacityTransferFunction AddPoint 170.0 0.101562
  colorTransferFunction AddRGBPoint 171.0 0.382174 0.517305 1
  opacityTransferFunction AddPoint 171.0 0.0755208
  colorTransferFunction AddRGBPoint 172.0 0.416113 0.565188 1
  opacityTransferFunction AddPoint 172.0 0.0624999
  colorTransferFunction AddRGBPoint 173.0 0.354816 0.656628 1
  opacityTransferFunction AddPoint 173.0 0.0494791
  colorTransferFunction AddRGBPoint 174.0 0.393282 0.554784 1
  opacityTransferFunction AddPoint 174.0 0.0364583
  colorTransferFunction AddRGBPoint 175.0 0.349027 0.669896 1
  opacityTransferFunction AddPoint 175.0 0.0234374
  colorTransferFunction AddRGBPoint 176.0 0.348891 0.740439 1
  opacityTransferFunction AddPoint 176.0 0.0234374
  colorTransferFunction AddRGBPoint 177.0 0.349908 0.803145 1
  opacityTransferFunction AddPoint 177.0 0.00390619
  colorTransferFunction AddRGBPoint 178.0 0.337694 0.86657 1
  opacityTransferFunction AddPoint 178.0 0.00390619
  colorTransferFunction AddRGBPoint 179.0 0.312909 0.860504 1
  opacityTransferFunction AddPoint 179.0 0.00390619
  colorTransferFunction AddRGBPoint 180.0 0.317491 0.929633 1
  opacityTransferFunction AddPoint 180.0 0.00390619
  colorTransferFunction AddRGBPoint 181.0 0.344972 1 0.978699
  opacityTransferFunction AddPoint 181.0 0.00390619
  colorTransferFunction AddRGBPoint 182.0 0.320156 1 0.927302
  opacityTransferFunction AddPoint 182.0 0.00390619
  colorTransferFunction AddRGBPoint 183.0 0.30623 1 0.954302
  opacityTransferFunction AddPoint 183.0 0.863281
  colorTransferFunction AddRGBPoint 184.0 0.288014 1 0.890506
  opacityTransferFunction AddPoint 184.0 0.82777
  colorTransferFunction AddRGBPoint 185.0 0.271639 1 0.897917
  opacityTransferFunction AddPoint 185.0 0.792258
  colorTransferFunction AddRGBPoint 186.0 0.294185 1 0.834452
  opacityTransferFunction AddPoint 186.0 0.756747
  colorTransferFunction AddRGBPoint 187.0 0.318737 1 0.827333
  opacityTransferFunction AddPoint 187.0 0.721236
  colorTransferFunction AddRGBPoint 188.0 0.307335 1 0.85859
  opacityTransferFunction AddPoint 188.0 0.685724
  colorTransferFunction AddRGBPoint 189.0 0.333009 1 0.854685
  opacityTransferFunction AddPoint 189.0 0.650213
  colorTransferFunction AddRGBPoint 190.0 0.365582 1 0.884833
  opacityTransferFunction AddPoint 190.0 0.614702
  colorTransferFunction AddRGBPoint 191.0 0.350816 1 0.871565
  opacityTransferFunction AddPoint 191.0 0.57919
  colorTransferFunction AddRGBPoint 192.0 0.409522 1 0.809174
  opacityTransferFunction AddPoint 192.0 0.543679
  colorTransferFunction AddRGBPoint 193.0 0.397073 1 0.843211
  opacityTransferFunction AddPoint 193.0 0.472656
  colorTransferFunction AddRGBPoint 194.0 0.447134 0.955037 1
  opacityTransferFunction AddPoint 194.0 0.453125
  colorTransferFunction AddRGBPoint 195.0 0.54736 0.977641 1
  opacityTransferFunction AddPoint 195.0 0.433594
  colorTransferFunction AddRGBPoint 196.0 0.471734 1 0.977254
  opacityTransferFunction AddPoint 196.0 0.414062
  colorTransferFunction AddRGBPoint 197.0 0.567048 0.89307 1
  opacityTransferFunction AddPoint 197.0 0.394531
  colorTransferFunction AddRGBPoint 198.0 0.547222 0.884563 1
  opacityTransferFunction AddPoint 198.0 0.375
  colorTransferFunction AddRGBPoint 199.0 0.679589 1 0.902872
  opacityTransferFunction AddPoint 199.0 0.355469
  colorTransferFunction AddRGBPoint 200.0 0.613689 0.757284 1
  opacityTransferFunction AddPoint 200.0 0.335937
  colorTransferFunction AddRGBPoint 201.0 0.75288 0.949348 1
  opacityTransferFunction AddPoint 201.0 0.316406
  colorTransferFunction AddRGBPoint 202.0 0.78574 0.81827 1
  opacityTransferFunction AddPoint 202.0 0.296875
  colorTransferFunction AddRGBPoint 203.0 0.83467 1 0.996246
  opacityTransferFunction AddPoint 203.0 0.257812
  colorTransferFunction AddRGBPoint 204.0 0.742244 0.775506 1
  opacityTransferFunction AddPoint 204.0 0.21875
  colorTransferFunction AddRGBPoint 205.0 1 0.718435 0.910984
  opacityTransferFunction AddPoint 205.0 0.200994
  colorTransferFunction AddRGBPoint 206.0 1 0.967058 0.918402
  opacityTransferFunction AddPoint 206.0 0.183239
  colorTransferFunction AddRGBPoint 207.0 1 0.684167 0.691559
  opacityTransferFunction AddPoint 207.0 0.165483
  colorTransferFunction AddRGBPoint 208.0 1 0.924797 0.647402
  opacityTransferFunction AddPoint 208.0 0.147727
  colorTransferFunction AddRGBPoint 209.0 1 0.653794 0.774229
  opacityTransferFunction AddPoint 209.0 0.129972
  colorTransferFunction AddRGBPoint 210.0 1 0.74007 0.62441
  opacityTransferFunction AddPoint 210.0 0.112216
  colorTransferFunction AddRGBPoint 211.0 1 0.750912 0.567449
  opacityTransferFunction AddPoint 211.0 0.0944602
  colorTransferFunction AddRGBPoint 212.0 1 0.753135 0.533339
  opacityTransferFunction AddPoint 212.0 0.0767045
  colorTransferFunction AddRGBPoint 213.0 1 0.929127 0.507911
  opacityTransferFunction AddPoint 213.0 0.0589488
  colorTransferFunction AddRGBPoint 214.0 1 0.673685 0.501791
  opacityTransferFunction AddPoint 214.0 0.0234374
  colorTransferFunction AddRGBPoint 215.0 1 0.881102 0.548802
  opacityTransferFunction AddPoint 215.0 0.0195312
  colorTransferFunction AddRGBPoint 216.0 1 0.954517 0.548119
  opacityTransferFunction AddPoint 216.0 0.0156249
  colorTransferFunction AddRGBPoint 217.0 1 0.905471 0.609673
  opacityTransferFunction AddPoint 217.0 0.0117187
  colorTransferFunction AddRGBPoint 218.0 0.665446 1 0.561791
  opacityTransferFunction AddPoint 218.0 0.00390619
  colorTransferFunction AddRGBPoint 219.0 0.68213 1 0.57204
  opacityTransferFunction AddPoint 219.0 0.00390619
  colorTransferFunction AddRGBPoint 220.0 0.642678 1 0.531367
  opacityTransferFunction AddPoint 220.0 0.00390619
  colorTransferFunction AddRGBPoint 221.0 0.52699 1 0.475384
  opacityTransferFunction AddPoint 221.0 0.00334816
  colorTransferFunction AddRGBPoint 222.0 0.583994 1 0.386336
  opacityTransferFunction AddPoint 222.0 0.00279014
  colorTransferFunction AddRGBPoint 223.0 0.538243 1 0.34327
  opacityTransferFunction AddPoint 223.0 0.00223211
  colorTransferFunction AddRGBPoint 224.0 0.527805 1 0.376427
  opacityTransferFunction AddPoint 224.0 0.00167408
  colorTransferFunction AddRGBPoint 225.0 0.502144 1 0.34921
  opacityTransferFunction AddPoint 225.0 0.00111605
  colorTransferFunction AddRGBPoint 226.0 0.518876 1 0.305165
  opacityTransferFunction AddPoint 226.0 0
  colorTransferFunction AddRGBPoint 227.0 0.597091 1 0.235693
  opacityTransferFunction AddPoint 227.0 0
  colorTransferFunction AddRGBPoint 228.0 0.477651 1 0.270124
  opacityTransferFunction AddPoint 228.0 0
  colorTransferFunction AddRGBPoint 229.0 0.547858 1 0.245083
  opacityTransferFunction AddPoint 229.0 0
  colorTransferFunction AddRGBPoint 230.0 0.487704 1 0.22763
  opacityTransferFunction AddPoint 230.0 0
  colorTransferFunction AddRGBPoint 231.0 0.487153 1 0.252077
  opacityTransferFunction AddPoint 231.0 0
  colorTransferFunction AddRGBPoint 232.0 0.59848 1 0.255782
  opacityTransferFunction AddPoint 232.0 0
  colorTransferFunction AddRGBPoint 233.0 0.539604 1 0.273016
  opacityTransferFunction AddPoint 233.0 0
  colorTransferFunction AddRGBPoint 234.0 0.595691 1 0.311273
  opacityTransferFunction AddPoint 234.0 0
  colorTransferFunction AddRGBPoint 235.0 0.573524 1 0.376418
  opacityTransferFunction AddPoint 235.0 0.00669641
  colorTransferFunction AddRGBPoint 236.0 0.55298 1 0.381817
  opacityTransferFunction AddPoint 236.0 0.0100446
  colorTransferFunction AddRGBPoint 237.0 0.623999 1 0.447465
  opacityTransferFunction AddPoint 237.0 0.0133928
  colorTransferFunction AddRGBPoint 238.0 0.643769 1 0.441653
  opacityTransferFunction AddPoint 238.0 0.016741
  colorTransferFunction AddRGBPoint 239.0 0.726844 1 0.568957
  opacityTransferFunction AddPoint 239.0 0.0200892
  colorTransferFunction AddRGBPoint 240.0 0.911989 1 0.705411
  opacityTransferFunction AddPoint 240.0 0.0234374
  colorTransferFunction AddRGBPoint 241.0 0.973976 0.573513 1
  opacityTransferFunction AddPoint 241.0 0.121094
  colorTransferFunction AddRGBPoint 242.0 0.893536 0.368641 1
  opacityTransferFunction AddPoint 242.0 0.169922
  colorTransferFunction AddRGBPoint 243.0 0.957021 0.320053 1
  opacityTransferFunction AddPoint 243.0 0.21875
  colorTransferFunction AddRGBPoint 244.0 1 0.106895 0.928777
  opacityTransferFunction AddPoint 244.0 0.557292
  colorTransferFunction AddRGBPoint 245.0 1 0.00727969 0.242058
  opacityTransferFunction AddPoint 245.0 0.726562
  colorTransferFunction AddRGBPoint 246.0 1 0.00248337 0.00511506
  opacityTransferFunction AddPoint 246.0 0.980469
  colorTransferFunction AddRGBPoint 247.0 1 0.00467116 0.0167705
  opacityTransferFunction AddPoint 247.0 0.980469
  colorTransferFunction AddRGBPoint 248.0 1 0.00689882 0.0805293
  opacityTransferFunction AddPoint 248.0 0.980469
  colorTransferFunction AddRGBPoint 249.0 1 0.00478619 0.0811463
  opacityTransferFunction AddPoint 249.0 0.980469
  colorTransferFunction AddRGBPoint 250.0 1 0.000280321 0.100206
  opacityTransferFunction AddPoint 250.0 0.980469
  colorTransferFunction AddRGBPoint 251.0 1 0.000177622 0.0579951
  opacityTransferFunction AddPoint 251.0 0.980469
  colorTransferFunction AddRGBPoint 252.0 1 0.00526476 0.119436
  opacityTransferFunction AddPoint 252.0 0.980469
  colorTransferFunction AddRGBPoint 253.0 1 0.000718892 0.0434636
  opacityTransferFunction AddPoint 253.0 0.954427
  colorTransferFunction AddRGBPoint 254.0 1 0.00719684 0.135113
  opacityTransferFunction AddPoint 254.0 0.941406
  colorTransferFunction AddRGBPoint 255.0 1 0.0461669 0.198018
  opacityTransferFunction AddPoint 255.0 0

# The property describes how the data will look
vtkVolumeProperty volumeProperty
    volumeProperty SetColor colorTransferFunction
    volumeProperty SetScalarOpacity opacityTransferFunction

# The mapper knows how to render the data
vtkFlowVRVolumeMapper volumeMapper
    volumeMapper SetInput [reader GetOutput]
    volumeMapper SetQuality 1
    volumeMapper SetSteps 100
#    volumeMapper SetShader shaders/volume_vtk_preint_fast_p.cg

# The volume holds the mapper and the property and
# can be used to position/orient the volume
vtkVolume volume
    volume SetMapper volumeMapper
    volume SetProperty volumeProperty

ren1 AddVolume volume

# The outline provides context for the data
vtkOutlineFilter outline
    outline SetInput [reader GetOutput]
vtkPolyDataMapper outlineMapper
    outlineMapper SetInput [outline GetOutput]
vtkActor outlineActor
    outlineActor SetMapper outlineMapper
    [outlineActor GetProperty] SetColor 0 0 0

#ren1 AddProp outlineActor

module Initialize

module Start renWin

# prevent the tk window from showing up then start the event loop
wm withdraw .

