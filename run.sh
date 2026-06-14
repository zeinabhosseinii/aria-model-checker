#!/bin/bash
cd "$(dirname "$0")"

# Use Java 26 (available on this machine)
export JAVA_HOME=/opt/homebrew/Cellar/openjdk/26.0.1/libexec/openjdk.jdk/Contents/Home
export PATH="$JAVA_HOME/bin:$PATH"

# Force recompile
touch src/main/java/erdg/Main.java
mvn compile -q 2>/dev/null

# Build classpath
DEPS=$(find ~/.m2/repository -name "*.jar" | grep -v "sources\|javadoc" | grep -E "antlr4-runtime|graphviz-java|slf4j|commons-exec|svgSalamander|nashorn|j2v8|viz\.js|jsr305|kitfox" | tr "\n" ":")

java -cp "target/classes:${DEPS}" erdg.Main "$@"
