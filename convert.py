from PIL import Image
import os
Image.MAX_IMAGE_PIXELS = None

def extract_rgb(path:str):
    image = Image.open(path)
    image = image.convert('RGB')
    data = image.tobytes()
    return data

rgb_names = [f.name.replace('.tif','') for f in os.scandir('img_rgb') if f.name.endswith('.rgb')]
for file in os.scandir('img_tif'):
    if file.name.endswith('.tif') and file.name.replace('.tif','') not in rgb_names:
        data = extract_rgb(file.path)
        with open(f'img_rgb/{file.name.replace(".tif","")}.rgb', 'wb') as f:
            f.write(data)