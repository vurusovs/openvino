# Copyright (C) 2018-2021 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

"""Main entry-point to run timetests tests.

Default run:
$ pytest test_timetest.py

Options[*]:
--test_conf     Path to test config
--exe           Path to timetest binary to execute
--niter         Number of times to run executable

[*] For more information see conftest.py
"""

from pathlib import Path
import logging
import os
import shutil

from scripts.run_timetest import run_timetest
from test_runner.utils import expand_env_vars

REFS_FACTOR = 1.2      # 120%


def test_timetest(instance, executable, niter, cl_cache_dir, model_cache_dir, test_info, temp_dir, validate_test_case,
                  prepare_db_info):
    """Parameterized test.

    :param instance: test instance. Should not be changed during test run
    :param executable: timetest executable to run
    :param niter: number of times to run executable
    :param cl_cache_dir: directory to store OpenCL cache
    :param model_cache_dir: directory to store IE model cache
    :param test_info: custom `test_info` field of built-in `request` pytest fixture
    :param temp_dir: path to a temporary directory. Will be cleaned up after test run
    :param validate_test_case: custom pytest fixture. Should be declared as test argument to be enabled
    :param prepare_db_info: custom pytest fixture. Should be declared as test argument to be enabled
    """
    # Prepare model to get model_path
    model_path = instance["model"].get("path")
    assert model_path, "Model path is empty"
    model_path = Path(expand_env_vars(model_path))

    # Copy model to a local temporary directory
    model_dir = temp_dir / "model"
    shutil.copytree(model_path.parent, model_dir)
    model_path = model_dir / model_path.name

    # Run executable
    exe_args = {
        "executable": Path(executable),
        "model": Path(model_path),
        "device": instance["device"]["name"],
        "niter": niter
    }
    logging.info("Run timetest once to generate any cache")
    retcode, msg, _, _ = run_timetest({**exe_args, "niter": 1}, log=logging)
    assert retcode == 0, f"Run of executable for warm up failed: {msg}"
    if cl_cache_dir:
        assert os.listdir(cl_cache_dir), "cl_cache isn't generated"
    if model_cache_dir:
        assert os.listdir(model_cache_dir), "model_cache isn't generated"

    retcode, msg, aggr_stats, raw_stats = run_timetest(exe_args, log=logging)
    assert retcode == 0, f"Run of executable failed: {msg}"

    # Add timetest results to submit to database and save in new test conf as references
    test_info["results"] = aggr_stats
    test_info["raw_results"] = raw_stats
