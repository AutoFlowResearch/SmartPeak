from __future__ import print_function
import os, sys, re
import git


def get_project_path():
  """Returns an absolute path to SmartPeak root directory."""
  return os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')

def get_version(filepath):
  """Reads current version from CMakeLists.txt configuration file.
  
  Parameters
  ----------
  filepath : str
    An absolute filepath to CMakeLists.txt

  Returns
  -------
  tuple
    A tuple of three integers (major, minor, patch)
  """
  major, minor, patch = 0, 0, 0
  with open(filepath) as config:
    content = config.read()
    major_match = re.findall(r'\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_MAJOR\s*([0-9]+)\s*\)[^\n]*', content)
    minor_match = re.findall(r'\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_MINOR\s*([0-9]+)\s*\)[^\n]*', content)
    patch_match = re.findall(r'\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_PATCH\s*([0-9]+)\s*\)[^\n]*', content)
    if len(major_match) > 0 and len(minor_match) > 0 and len(patch_match) > 0:
      major, minor, patch = int(major_match[0]), int(minor_match[0]), int(patch_match[0])
  return major, minor, patch

def compare_version(lhs, rhs):
  """Compare two versions.
  
  Parameters
  ----------
  lhs : tuple
    tuple of three integers (major, minor, patch)
  rhs : tuple
    tuple of three integers (major, minor, patch)

  Returns
  -------
  int
    Returns 0 if lhs and rhs are equal, 1 if lhs is bigger than rhs, otherwise -1. 
  """
  if lhs == rhs:
    return 0
  elif lhs > rhs:
    return 1
  else:
    return -1

def update_version(config_file, version):
  """Modifies current version of SmartPeak package to version given in parameter.

  Parameters
  ----------
  filepath : str
    An absolute filepath to CMakeLists.txt
  version : tuple
    A tuple (major, minor, patch)

  Returns
  -------
  boolean
    True on success, False otherwise
  """
  major, minor, patch = get_version(config_file)
  n_major, n_minor, n_patch = version

  # Check if the new version is larger or equal than previous:
  if compare_version((n_major, n_minor, n_patch), (major, minor, patch)) < 0:
    print("Given version '{}.{}.{}' has to be larger or equal than the previous version of the package '{}.{}.{}'".format(
      n_major, n_minor, n_patch, major, minor, patch), file=sys.stderr)
    return False
  
  # Modify the tag in configuration file:
  with open(config_file, 'r') as config:
    lines = config.readlines()

    major_reg = r'^\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_MAJOR\s+([0-9]+)\s*\)[^\n]*$'
    minor_reg = r'^\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_MINOR\s+([0-9]+)\s*\)[^\n]*$'
    patch_reg = r'^\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_PATCH\s+([0-9]+)\s*\)[^\n]*$'

    for i in range(len(lines)):
      if re.match(major_reg, lines[i]):
        res = re.split(r'^\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_MAJOR\s+([0-9]+)\s*', lines[i])
        endl = res[2]
        lines[i] = ('set(SMARTPEAK_PACKAGE_VERSION_MAJOR %d' + endl) % n_major
      elif re.match(minor_reg, lines[i]):
        res = re.split(r'^\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_MINOR\s+([0-9]+)\s*', lines[i])
        endl = res[2]
        lines[i] = ('set(SMARTPEAK_PACKAGE_VERSION_MINOR %d' + endl) % n_minor
      elif re.match(patch_reg, lines[i]):
        res = re.split(r'^\s*[Ss][Ee][Tt]\s*\(\s*SMARTPEAK_PACKAGE_VERSION_PATCH\s+([0-9]+)\s*', lines[i])
        endl = res[2]
        lines[i] = ('set(SMARTPEAK_PACKAGE_VERSION_PATCH %d' + endl) % n_patch
    
    # Override version:
    with open(config_file, 'w') as new_config:
      new_config.writelines(lines)

  return True

def get_new_version(set_version, bump, current_version):
  """Based on current version, depending on the option, returns updated version.
  It does not change any file.

  Parameters
  ----------
  set_version : str
    A string containing valid version or None.
  bump : str
    A string containing which component to bump (or empty string)
  current_version : tuple
    A tuple (major, minor, patch)

  Returns
  -------
  tuple
    A tuple containing (success, (major, minor, patch))
  """
  major, minor, patch = current_version
  n_major, n_minor, n_patch = 0, 0, 0
  # Use set-version flag to override bump option:
  tag_filter_regexp = r'^v?([0-9]+)\.([0-9]+)\.([0-9]+)$'
  if set_version != 'None':
    ver_check = re.match(tag_filter_regexp, set_version)
    if not ver_check:
      print("Incorrect version tag, make sure the tag matches a pattern (v)MAJOR.MINOR.PATCH", file=sys.stderr)
      return False, (0, 0, 0)
    else:
      a, b, c = ver_check.groups()
      n_major, n_minor, n_patch = int(a), int(b), int(c)
  else:
    n_major, n_minor, n_patch = bumped_version(bump, current_version)
  return True, (n_major, n_minor, n_patch)

def bumped_version(bump, current_version):
  """Returns bumped version.

  Parameters
  ----------
  bump : str
    Which component of version should be incremented (major, minor, patch).
  current_version : tuple
    A tuple (major, minor, patch)

  Returns
  -------
  boolean
    True on success, False otherwise
  """
  major, minor, patch = current_version
  n_major, n_minor, n_patch = major, minor, patch
  if bump == 'major':
    n_major += 1
  elif bump == 'minor':
    n_minor += 1
  elif bump == 'patch':
    n_patch += 1
  return n_major, n_minor, n_patch

def get_release_command_pattern():
  """Returns regex pattern for matching a release command (used with git commit messages)"""
  return r'\(\s*release\s*(((bump)\s*(major|minor|patch))|((set)\s*v?(([0-9]+)\.([0-9]+)\.([0-9]+))))\s*\)'

def is_release_command_present(message):
  """Reads a message string and tells whether there is a release command present.

  Parameters
  ----------
  message : str
    A string, e.g. commit message.

  Returns
  -------
  tuple
    Returns (type, value), where type can be ('bump'|'set') and 
    value can be ('major'|'minor'|'patch'|'$major.$minor.$patch'). 
    If pattern doesn't match it returns tuple of empty strings.
  """
  pattern = get_release_command_pattern()
  p = re.findall(pattern, message)
  if p:
      if p[0][2] and p[0][3]:
        return p[0][2], p[0][3]
      elif p[0][5] and p[0][6]:
        return p[0][5], p[0][6]
  return '', ''

def get_latest_commit_message():
  """Retrieves the latest commit message form the repository."""
  repo = git.Repo(get_project_path())
  return repo.head.commit.message

def read_changes(filepath, version):
  """Retrieves changelog information for given version.

  Parameters
  ----------
  filepath : str
    An absolute path to changelog file.
  version : tuple
    A tuple (major, minor, patch)

  Returns
  -------
  str
    A content of changelog for given version
    or None if version not included in the file.
  """
  pattern = r'''(?ismx)
^(v?[0-9]+\.[0-9]+(?:\.[0-9]+)?[a-z]*)\s+\([0-9]+-[0-9]{2}-[0-9]{2}\)(?:\n|\r\n|\r)
--+(?:\n|\r\n|\r)
\s*(.*?)
(?=(?:v?[0-9]+\.[0-9]+(?:\.[0-9]+)?[a-z]*)\s+\([0-9]+-[0-9]{2}-[0-9]{2}\)(?:\n|\r\n|\r)
--+(?:\n|\r\n|\r)|\Z)'''

  with open(filepath, 'r') as changelog:
    lines = changelog.read()
    f = re.findall(pattern, lines)
    if f:
      for results in f:
        tag = results[0]
        content = results[1]
        if tag == version or version == 'latest':
          return content
  return None

def preprocess_changes(content):
  """Changes reStructuredText format to markdown (for github releases formatting).

  Parameters
  ----------
  content : str
    A raw changelog content for given version.

  Returns
  -------
  str
    The same text formatted with markdown.
  """
  lines = content.rstrip("\n").split('\n')
  for i, line in enumerate(lines):
    f = re.findall(r'~~+(?:\n|\r\n|\r)?', lines[i])
    if f:
      lines[i - 1] = '# ' + lines[i - 1]
      del lines[i]
  return '\n'.join(lines)
