
data_args = []
with open('metadata.txt') as f:
    metadata = f.readlines()
    for datum in metadata:
        line_data = datum.split(';')
        name = line_data[0]
        target_file = f'img_rgb/{name}.rgb'
        width = line_data[1]
        lenght = line_data[2].replace('\n','')
        data_args.append((target_file, width, lenght))

statements = []
executable_name = 'pixelate'
nchannels = 3
nprocessors = 4
window_width = 16
window_height = 16
for data in data_args:
    statements.append(f'mpirun -np {nprocessors} {executable_name} {data[0]} {data[1]} {data[2]} {nchannels} {window_width} {window_height}')
with open(f'exec_{nprocessors}-{window_width}.sh', 'w') as f:
    f.write('#!/bin/bash\n')
    for statement in statements:
        f.write(statement + '\n')