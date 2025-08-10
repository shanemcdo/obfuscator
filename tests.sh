#!/bin/zsh

INPUT_FILE=/tmp/input.c
EXPECTED_FILE=/tmp/expected.c
ACTUAL_FILE=/tmp/actual.c

fail_count=0

run_test() {
	bin/test "$INPUT_FILE" > "$ACTUAL_FILE"
	diff="$(diff "$ACTUAL_FILE" "$EXPECTED_FILE")"
	rc="$?"
	if [ -n "$diff" ]; then
		(( fail_count ++ ))
		echo "Test Failed with code $rc"
		echo "----------------------------------------"
		cat "$INPUT_FILE"
		echo "----------------------------------------"
		echo $diff
		echo "----------------------------------------"
	else
		printf '.'
	fi
}

cat > "$INPUT_FILE" <<EOF
EOF
cat > "$EXPECTED_FILE" <<EOF
EOF
run_test

cat > "$INPUT_FILE" <<EOF
int main(){
	return 0;
}
EOF
cat > "$EXPECTED_FILE" <<EOF
#define e int
#define ee main
#define eee (
#define eeee )
#define eeeee {
#define eeeeee return
#define eeeeeee 0
#define eeeeeeee ;
#define eeeeeeeee }
e ee eee eeee eeeee
 eeeeee eeeeeee eeeeeeee
 eeeeeeeee
EOF
run_test

cat > "$INPUT_FILE" <<EOF
#ifdef DEBUG
puts("This is a test");
#endif
EOF
cat > "$EXPECTED_FILE" <<EOF
#define e puts
#define ee (
#define eee "This is a test"
#define eeee )
#define eeeee ;
#ifdef DEBUG
e ee eee eeee eeeee
#endif
EOF
run_test

echo "\n$fail_count failed tests"
