pipeline {
    agent any

    stages {
        stage('build') {
            steps {
                    sh 'cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_CODECOVERAGE=on && make -j8'
            }
        }
        stage('test') {
            steps {
                sh 'make codecoverage'
                junit 'tests/gtestresults.xml'
            }
            post {
                success {
                  // publish html
                  publishHTML target: [
                      allowMissing: false,
                      alwaysLinkToLastBuild: false,
                      keepAll: true,
                      reportDir: 'tests/codecoverage',
                      reportFiles: 'index.html',
                      reportName: 'Code Coverage Report'
                    ]
                }
            }
        }
    }
}
