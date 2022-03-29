from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw

'''
# Plot question mark:
img = Image.new('RGB', (500,500), (250,250,250))
draw = ImageDraw.Draw(img)
font = ImageFont.truetype("./font/OpenSans-Regular.ttf", 400)
draw.text((180, -30),"?",(0,0,0),font=font)
img.save('question_mark_img.jpg')
'''

'''
# plot digit numbers (from 0 to 9):
for i in range(10):
    img = Image.new('RGB', (500,500), (250,250,250))
    draw = ImageDraw.Draw(img)
    font = ImageFont.truetype("./font/OpenSans-Regular.ttf", 400)
    draw.text((150, -30),str(i),(0,0,0),font=font)
    img.save('digit_number_img_'+str(i)+'.jpg')
'''

# plot digit numbers (from 0 to 9):
for i in range(10):
    img = Image.new('RGB', (28,28), (250,250,250))
    draw = ImageDraw.Draw(img)
    font = ImageFont.truetype("/home/matthias/OCR/Python/number_generator_training/font/OpenSans-Regular.ttf", 20)
    draw.text((8, 0),str(i),(255,0,0),font=font)
    img.save(str(i)+'.jpg')

img = Image.new('RGB', (28,28), (250,250,250))
img.save(str(0)+'.jpg')