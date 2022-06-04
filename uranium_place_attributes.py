from glob import glob
import os
import re
from tqdm import tqdm
from pathlib import Path


ignored = ['target']


def open_file(filename, mode='r'):
    return open(filename, mode, newline='\n')


def process_rust_file(filename):
    try:
        backup_lines = []
        new_lines = []
        usings = [
            'use uranium_backend::*;\n',
            'use uranium_backend::interface::FunctionData;\n',
            'use uranium_prof_macro::profile_func;\n',
            ''
        ]
        is_first = True
        with open_file(filename) as f:
            for line in f:
                backup_lines.append(line)
                if line.strip() == '' or '//' in line or 'extern' in line or (len(new_lines) > 0 and 'extern' in new_lines[-1]):
                    new_lines.append(line)
                    continue
                nline = re.sub(r'((pub(\(\w+\))?\s*)?(unsafe\s*)?(async\s*)?fn\s+\w+)', '#[profile_func]\n\\1', line)
                if is_first:
                    new_lines += usings
                    is_first = False
                new_lines.append(nline)
        with open_file(filename, 'w') as f:
            f.writelines(new_lines)
        with open_file(filename + '.backup', 'w') as f:
            f.writelines(backup_lines)
    except:
        return


p1 = str(Path('../uranium-profiler/uranium-backend'   ).resolve()).replace(os.sep, os.sep * 2)
p2 = str(Path('../uranium-profiler/uranium-prof-macro').resolve()).replace(os.sep, os.sep * 2)


def process_cargo_file(filename):
    dependencies = [
        'uranium_backend    = { path = "' + p1 + '" }\n',
        'uranium_prof_macro = { path = "' + p2 + '" }\n'
    ]
    backup_lines = []
    new_lines = []
    with open_file(filename) as f:
        for line in f:
            backup_lines.append(line)
            if len(new_lines) > 0 and new_lines[-1].strip() == '[dependencies]':
                new_lines += dependencies
            new_lines.append(line)
    with open_file(filename, 'w') as f:
        f.writelines(new_lines)
    with open_file(filename + '.backup', 'w') as f:
        f.writelines(backup_lines)


def backup_file(filename):
    if not os.path.isfile(filename + '.backup'):
        return
    with open_file(filename + '.backup') as f:
        backup_lines = f.readlines()
    with open_file(filename, 'w') as f:
        f.writelines(backup_lines)
    os.remove(filename + '.backup')


def process_file(filename):
    for i in ignored:
        if i + os.sep in filename:
            return
    if os.path.isfile(filename + '.backup'):
        raise 'Backup file exists'
    if filename.endswith('.rs'):
        process_rust_file(filename)
    else:
        process_cargo_file(filename)


def main():
    f = backup_file if input('Backup (y/n): ').lower() == 'y' else process_file
    print('Searching for files...')
    filenames = glob('**/*.rs', recursive=True) + glob('**/Cargo.toml', recursive=True)
    print(f'{len(filenames)} will be processed')
    for filename in tqdm(filenames):
        f(filename)
    print('Finished')
    print()


if __name__ == '__main__':
    main()
