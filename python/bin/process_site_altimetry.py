"""Main script for altimetry processing from Code_TYMPAN GUI"""

import sys
import logging

from tympan.models.project import Project


def set_logger(fpath='tympan.log'):
    """Configure logging"""
    logging.basicConfig(filename=fpath, level=logging.DEBUG,
                        format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')


def main(input_project, result_file):
    """Process altimetry from `input_project` and save to `result_file` (PLY
    format).
    """
    try:
        project = Project.from_xml(input_project, verbose=True)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    project.export_altimetry(result_file)

if __name__ == '__main__':
    set_logger()
    if len(sys.argv) != 3:
        err = "%s called with bad arguments." % __file__
        logging.error("%s Couldn't process altimetry.", err)
        sys.stderr.write('Error: ' + err)
        sys.exit(-1) # XXX to be improved
    input_project = sys.argv[1]
    result_file = sys.argv[2]
    main(input_project, result_file)
