import unittest
import os

import smartpeak.utils as utils


class UtilsTest(unittest.TestCase):

    def copy_file(self, org_path, dest_path):
        with open(org_path) as org, open(dest_path, 'w') as dest:
            content = org.read()
            dest.write(content)

    def test_get_version(self):
        filename = 'ExampleCMakeLists.txt'
        major, minor, patch = utils.get_version(
            os.path.join(utils.get_project_path(), 'tools', 'smartpeak', 'test', 'data', filename))
        self.assertEqual((major, minor, patch), (1, 4, 3))

    def test_compare_version(self):
        versions = [(1,1,1), (1,1,2), (1,0,1), (2,1,1)]
        self.assertEqual(utils.compare_version(versions[0], versions[0]), 0)
        self.assertEqual(utils.compare_version(versions[1], versions[0]), 1)
        self.assertEqual(utils.compare_version(versions[0], versions[1]), -1)
        self.assertEqual(utils.compare_version(versions[0], versions[2]), 1)
        self.assertEqual(utils.compare_version(versions[2], versions[0]), -1)
        self.assertEqual(utils.compare_version(versions[3], versions[0]), 1)
        self.assertEqual(utils.compare_version(versions[0], versions[3]), -1)

    def test_update_version(self):
        dirpath = os.path.join(utils.get_project_path(),
            'tools', 'smartpeak', 'test', 'data')
        # First copy file:
        org_filepath = os.path.join(dirpath, 'ExampleCMakeLists.txt')
        dest_filepath = os.path.join(dirpath, 'ExampleCMakeLists1.txt')
        self.copy_file(org_filepath, dest_filepath)
        # Smaller version case
        version = (1, 4, 2)
        success = utils.update_version(dest_filepath, version)
        self.assertFalse(success)
        # Equal versions case
        self.copy_file(org_filepath, dest_filepath)
        version = (1, 4, 3)
        success = utils.update_version(dest_filepath, version)
        major, minor, patch = utils.get_version(dest_filepath)
        self.assertTrue(success)
        self.assertEqual(version, (major, minor, patch))
        # Higher version case
        self.copy_file(org_filepath, dest_filepath)
        version = (1, 5, 4)
        success = utils.update_version(dest_filepath, version)
        major, minor, patch = utils.get_version(dest_filepath)
        self.assertTrue(success)
        self.assertEqual(version, (major, minor, patch))

    def test_get_new_version(self):
        version = (1,3,2)
        self.assertEqual(utils.get_new_version('None', 'patch', version), (True, (1,3,3)))
        self.assertEqual(utils.get_new_version('None', 'minor', version), (True, (1,4,2)))
        self.assertEqual(utils.get_new_version('None', 'major', version), (True, (2,3,2)))
        self.assertEqual(utils.get_new_version('wrong_version', '', version), (False, (0,0,0)))
        self.assertEqual(utils.get_new_version('1.3.3', '', version), (True, (1,3,3)))

    def test_bumped_version(self):
        major, minor, patch = (1,3,2)
        version = (major, minor, patch)
        self.assertEqual(utils.bumped_version('patch', version), (major, minor, patch + 1))
        self.assertEqual(utils.bumped_version('minor', version), (major, minor + 1, patch))
        self.assertEqual(utils.bumped_version('major', version), (major + 1, minor, patch))

    def test_is_release_command_present(self):
        message = "Some string"
        self.assertEqual(utils.is_release_command_present(message), ('', ''))
        message = "Update changelog (release bump patch)"
        self.assertEqual(utils.is_release_command_present(message), ('bump', 'patch'))
        message = "Update changelog (release bump minor)"
        self.assertEqual(utils.is_release_command_present(message), ('bump', 'minor'))
        message = "Update changelog (release bump major)"
        self.assertEqual(utils.is_release_command_present(message), ('bump', 'major'))
        message = "Update changelog (release set 1.3.3)"
        self.assertEqual(utils.is_release_command_present(message), ('set', '1.3.3'))

    def test_read_changes(self):
        dirpath = os.path.join(utils.get_project_path(),
            'tools', 'smartpeak', 'test', 'data')
        filepath = os.path.join(dirpath, 'CHANGELOG.rst')

        self.assertEqual(utils.read_changes(filepath, 'v1.3.0'), None)
        expected_content = 'Features\n~~~~~~~~\n- New feature\n\n\n'
        self.assertEqual(utils.read_changes(filepath, 'v1.2.0'), expected_content)
        expected_content = 'Fix\n~~~\n- Fix\n\n\n'
        self.assertEqual(utils.read_changes(filepath, 'v1.1.0'), expected_content)
        expected_content = 'Changes\n~~~~~~~\n- New change\n\n\n'
        self.assertEqual(utils.read_changes(filepath, 'v1.0.0'), expected_content)

    def test_preprocess_changes(self):
        content = 'Features\n~~~~~~~~\n- New feature\n\n\n'
        self.assertEqual(utils.preprocess_changes(content), '# Features\n- New feature')
        content = 'Features\n~~~~~~~~\n- New feature\n\nFix\n~~~\n-Fix'
        self.assertEqual(utils.preprocess_changes(content), '# Features\n- New feature\n\n# Fix\n-Fix')
        content = 'Features\n~~~~~~~~\n- New feature\n\nFix\n~~~\n-Fix\n\nChanges\n~~~~~~~\n- New change'
        self.assertEqual(utils.preprocess_changes(content), 
            '# Features\n- New feature\n\n# Fix\n-Fix\n\n# Changes\n- New change')
        

if __name__ == "__main__":
    unittest.main()
