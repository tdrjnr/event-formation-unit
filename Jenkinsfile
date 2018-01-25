project = "efu"

images = [
    'centos7': [
        'name': 'essdmscdm/centos7-build-node:1.0.1',
        'sh': 'sh'
    ],
    'centos7-gcc6': [
        'name': 'essdmscdm/centos7-gcc6-build-node:1.0.0',
        'sh': '/usr/bin/scl enable rh-python35 devtoolset-6 -- /bin/bash'
    ],
    'fedora25': [
        'name': 'essdmscdm/fedora25-build-node:1.0.0',
        'sh': 'sh'
    ],
    'ubuntu1604': [
        'name': 'essdmscdm/ubuntu16.04-build-node:2.0.0',
        'sh': 'sh'
    ],
    'ubuntu1710': [
        'name': 'essdmscdm/ubuntu17.10-build-node:1.0.0',
        'sh': 'sh'
    ]
]

base_container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "event-formation-unit: " + failureMessage
    throw exception_obj
}

def Object container_name(image_key) {
    return "${base_container_name}-${image_key}"
}

def docker_dependencies(image_key) {
    def conan_remote = "ess-dmsc-local"
    def custom_sh = images[image_key]['sh']
    sh """docker exec ${container_name(image_key)} ${custom_sh} -c \"
        mkdir build
        cd build
        conan remote add \
            --insert 0 \
            ${conan_remote} ${local_conan_server}
        conan install --build=outdated ../${project}/conanfile.txt
    \""""
}

def docker_cmake(image_key) {
    cmake_exec = "cmake"
    def custom_sh = images[image_key]['sh']
    sh """docker exec ${container_name(image_key)} ${custom_sh} -c \"
        cd build
        . ./activate_run.sh
        ${cmake_exec} --version
        ${cmake_exec} -DCOV=1 ../${project}
    \""""
}

def docker_build(image_key) {
    def custom_sh = images[image_key]['sh']
    sh """docker exec ${container_name(image_key)} ${custom_sh} -c \"
        cd build
        . ./activate_run.sh
        make --version
        make VERBOSE=ON
    \""""
}

def docker_tests(image_key) {
    def custom_sh = images[image_key]['sh']
    dir("${project}/tests") {
        try {
            sh """docker exec ${container_name(image_key)} ${custom_sh} -c \"
                cd build
                . ./activate_run.sh
                make runtest VERBOSE=ON
                make coverage_xml
            \""""
        } catch(e) {
            sh "docker cp ${container_name(image_key)}:/home/jenkins/build/test_results/*.xml ."
            junit '*.xml'
            failure_function(e, 'Run tests (${container_name(image_key)}) failed')
        }
    }
}

def Object get_container(image_key) {
    def image = docker.image(images[image_key]['name'])
    def container = image.run("\
        --name ${container_name(image_key)} \
        --tty \
        --network=host \
        --env http_proxy=${env.http_proxy} \
        --env https_proxy=${env.https_proxy} \
        --env local_conan_server=${env.local_conan_server} \
        ")
    return container
}

def get_pipeline(image_key)
{
    return {
        stage("${image_key}") {
            try {
                def container = get_container(image_key)
                def custom_sh = images[image_key]['sh']

                // Copy sources to container and change owner and group.
                dir("${project}") {
                    sh "docker cp code ${container_name(image_key)}:/home/jenkins/${project}"
                    sh """docker exec --user root ${container_name(image_key)} ${custom_sh} -c \"
                        chown -R jenkins.jenkins /home/jenkins/${project}
                        \""""
                }

                try {
                    docker_dependencies(image_key)
                } catch (e) {
                    failure_function(e, "Get dependencies for ${image_key} failed")
                }

                try {
                    docker_cmake(image_key)
                } catch (e) {
                    failure_function(e, "CMake for ${image_key} failed")
                }

                try {
                    docker_build(image_key)
                } catch (e) {
                    failure_function(e, "Build for ${image_key} failed")
                }

                docker_tests(image_key)
            } catch(e) {
                failure_function(e, "Unknown build failure for ${image_key}")
            } finally {
                sh "docker stop ${container_name(image_key)}"
                sh "docker rm -f ${container_name(image_key)}"
            }
        }
    }
}

def get_osx_pipeline()
{
    return {
        stage("MacOSX") {
            node ("macos") {
            // Delete workspace when build is done
                cleanWs()

                dir("${project}/code") {
                    try {
                        checkout scm
                    } catch (e) {
                        failure_function(e, 'MacOSX / Checkout failed')
                    }
                }

                dir("${project}/build") {
                    try {
                        sh "conan install --build=outdated ../code/conanfile.txt "
                    } catch (e) {
                        failure_function(e, 'MacOSX / getting dependencies failed')
                    }

                    try {
                        sh "cmake ../code"
                    } catch (e) {
                        failure_function(e, 'MacOSX / CMake failed')
                    }

                    try {
                        sh "make runtest"
                    } catch (e) {
                        failure_function(e, 'MacOSX / build failed')
                    }
                }

            }
        }
    }
}

node('docker && dmbuild03.dm.esss.dk') {

    // Delete workspace when build is done
    cleanWs()

    stage('Checkout') {
        dir("${project}/code") {
            try {
                scm_vars = checkout scm
            } catch (e) {
                failure_function(e, 'Checkout failed')
            }
        }
    }

    def builders = [:]
    builders['centos7'] = get_pipeline('centos7')
    builders['centos7-gcc6'] = get_pipeline('centos7-gcc6')
    //builders['fedora25'] = get_pipeline('fedora25')
    //builders['ubuntu1604'] = get_pipeline('ubuntu1604')
    //builders['MocOSX'] = get_osx_pipeline()

    /*
    for (x in images.keySet()) {
        def image_key = x
        builders[image_key] = get_pipeline(image_key)
    }
    */
    parallel builders
}

node('kafka-client && centos7') {
    cleanWs()

    dir("code") {
        stage("Checkout") {
            try {
                checkout scm
            } catch (e) {
                failure_function(e, 'Checkout failed')
            }
        }

        stage("Static analysis") {
            try {
                sh "cloc --by-file --xml --out=cloc.xml ."
                sh "xsltproc jenkins/cloc2sloccount.xsl cloc.xml > sloccount.sc"
                sloccountPublish encoding: '', pattern: ''
            } catch (e) {
                failure_function(e, 'Static analysis failed')
            }
        }
    }


    dir("build") {
        stage("Build") {
            try {
                sh "cmake -DCOV=1 ../code"
                sh "make -j 5 VERBOSE=ON"
            } catch (e) {
                failure_function(e, 'Failed to compile')
            }
        }

        stage("Run tests") {
            try {
                sh "make -j 5 runtest VERBOSE=ON"
                junit 'test_results/*.xml'
                sh "make coverage_xml"
                step([
                    $class: 'CoberturaPublisher',
                    autoUpdateHealth: true,
                    autoUpdateStability: true,
                    coberturaReportFile: 'coverage/coverage.xml',
                    failUnhealthy: false,
                    failUnstable: false,
                    maxNumberOfBuilds: 0,
                    onlyStable: false,
                    sourceEncoding: 'ASCII',
                    zoomCoverageChart: false
                ])
            } catch (e) {
                junit 'test_results/*.xml'
                failure_function(e, 'Unit tests failed')
            }
        }
    }
}
