
import os
import shutil

# Various compression tools from stdlib
import zipfile, tarfile, bz2, lzma, gzip, io

# python -m pip install --user requests
import requests


def dl_archive_to(url, dst_path, extension=None):
  print('downloading {} to {}'.format(url, dst_path))

  if extension is None:
    if url.endswith('.zip') or url.endswith('.jar'):
      extension = '.zip'
    elif url.endswith('.tar.bz2'):
      extension = '.tar.bz2'
    elif url.endswith('.tar.xz'):
      extension = '.tar.xz'
    elif url.endswith('.txz'):
      extension = '.tar.xz'
    elif url.endswith('.7z'):
      extension = '.7z'
    else:
      extension = url
  
  if extension.endswith('.zip'):
    if os.path.exists(url):
      with open(url, 'rb') as zip_content:
        zip_mem = zipfile.ZipFile(io.BytesIO(zip_content))
        if not os.path.exists(dst_path):
          os.makedirs(dst_path)
        print('extracting to {}'.format(dst_path))
        zip_mem.extractall(dst_path)

    else:
      zip_r = requests.get(url)
      zip_mem = zipfile.ZipFile(io.BytesIO(zip_r.content))
      if not os.path.exists(dst_path):
        os.makedirs(dst_path)
      print('extracting to {}'.format(dst_path))
      zip_mem.extractall(dst_path)

  elif extension.endswith('.tar.bz2'):
    if os.path.exists(url):
      with open(url, 'rb') as tar_bz2_content:
        tar_bz2_bytes = tar_bz2_content.read()
        tar_mem = tarfile.open(
          fileobj=io.BytesIO(bz2.decompress( tar_bz2_bytes ))
        )
        if not os.path.exists(dst_path):
          os.makedirs(dst_path)
        print('extracting to {}'.format(dst_path))
        tar_mem.extractall(dst_path)

    else:
      tar_bz2_r = requests.get(url)
      tar_mem = tarfile.open(
        fileobj=io.BytesIO(bz2.decompress( tar_bz2_r.content ))
      )
      if not os.path.exists(dst_path):
        os.makedirs(dst_path)
      print('extracting to {}'.format(dst_path))
      tar_mem.extractall(dst_path)

  elif extension.endswith('.tar.xz'):
    if os.path.exists(url):
      with open(url, 'rb') as tar_xz_content:
        tar_xz_bytes = tar_xz_content.read()
        tar_mem = tarfile.open(
          fileobj=io.BytesIO(lzma.decompress( tar_xz_bytes ))
        )
        if not os.path.exists(dst_path):
          os.makedirs(dst_path)
        print('extracting to {}'.format(dst_path))
        tar_mem.extractall(dst_path)

    else:
      tar_xz_r = requests.get(url)
      tar_mem = tarfile.open(
        fileobj=io.BytesIO(lzma.decompress( tar_xz_r.content ))
      )
      if not os.path.exists(dst_path):
        os.makedirs(dst_path)
      print('extracting to {}'.format(dst_path))
      tar_mem.extractall(dst_path)

  elif extension.endswith('.tar.gz'):
    if os.path.exists(url):
      with open(url, 'rb') as tar_gz_content:
        tar_gz_bytes = tar_gz_content.read()
        tar_mem = tarfile.open(
          fileobj=io.BytesIO(gzip.decompress( tar_gz_bytes ))
        )
        if not os.path.exists(dst_path):
          os.makedirs(dst_path)
        print('extracting to {}'.format(dst_path))
        tar_mem.extractall(dst_path)

    else:
      tar_gz_r = requests.get(url)
      tar_mem = tarfile.open(
        fileobj=io.BytesIO(gzip.decompress( tar_gz_r.content ))
      )
      if not os.path.exists(dst_path):
        os.makedirs(dst_path)
      print('extracting to {}'.format(dst_path))
      tar_mem.extractall(dst_path)

  elif extension.endswith('.7z'):
    if os.path.exists(url):
      with py7zr.SevenZipFile(url, mode='r') as archive:
        print('extracting to {}'.format(dst_path))
        archive.extractall(path=dst_path)

    else:
      tmp_f = tempfile.NamedTemporaryFile(mode='w+b', delete=False)
      tmp_f.close()

      sevenZ_r = requests.get(url)
      
      with open(tmp_f.name, 'wb') as fd:
        fd.write(sevenZ_r.content)

      with py7zr.SevenZipFile(tmp_f.name, mode='r') as archive:
        print('extracting to {}'.format(dst_path))
        archive.extractall(path=dst_path)

      os.unlink(tmp_f.name)

  else:
    raise Exception("Unknown archive type: {}".format(url))

  # We move files up until there is more than 1 item at the root (dst_path)
  # This avoids messy issues where we extract to "ABC/" and get
  # "ABC/ABC-1.2.3/<actual stuff we wanted under ABC>"
  remaining_loops = 5
  while len(os.listdir(dst_path)) < 2 and remaining_loops > 0:
    remaining_loops -= 1
    # Move everything in dst_path/<directory>/* into dst_path
    child_dir = os.path.join(dst_path, os.listdir(dst_path)[0])
    for child_f in os.listdir(child_dir):
      shutil.move(os.path.join(child_dir, child_f), os.path.join(dst_path, child_f))
    os.rmdir(child_dir)

