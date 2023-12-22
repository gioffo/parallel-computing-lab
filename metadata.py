from PIL import Image
import os
Image.MAX_IMAGE_PIXELS = None

with open('metadata.txt', 'w') as metadata:
    for f in os.scandir('img_tif'):
        if f.name.endswith('.tif'):
            im = Image.open(f.path)
            width, height = im.size
            name_without_ext = os.path.splitext(f.name)[0]
            metadata.write(name_without_ext + ';' + str(width) + ';' + str(height) + '\n')