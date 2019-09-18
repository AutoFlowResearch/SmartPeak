#!groovy
pipeline {
    agent any
    environment {
        PATH = "$PATH;C:\\OpenMS\\build\\bin\\Debug;C:\\Qt\\5.12\\msvc2017_64\\bin;C:\\SDL2-2.0.9\\lib\\x64"
        BOOST_TEST_DETECT_MEMORY_LEAK=0
    }
    stages {
        stage('Build') {
            steps {
                bat 'ci/build_smartpeak.bat'
            }
        }
        stage('Tests') {
            steps {
                bat 'ci/run_tests.bat'
            }
        }
        // stage('Examples') {
        //     steps {
        //         sh '.circleci/run_examples.sh build/bin/Debug C:/cygwin64/bin/'
        //     }
        // }
    }
    post {
        always {
            echo 'Saving artifacts'
            archiveArtifacts artifacts: 'build/bin/**/*.exe', fingerprint: true
            // echo 'Generating reports'
            // junit 'build/Testing/**/*.xml'
            deleteDir()
        }
    }
}
