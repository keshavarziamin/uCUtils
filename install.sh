#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAIN_FILE="${ROOT}/cmake/gcc-arm-none-eabi.cmake"

ARCH="host"
PREFIX=""
BUILD_TYPE="Debug"
BUILD_EXAMPLES="OFF"
RUN_TESTS="auto"
LINKER_SCRIPT=""

usage() {
  cat <<EOF
Usage: $(basename "$0") [options]

Configure, build, test, and install uCUtils with CMake.

Options:
  --arch ARCH           host | cortex-m0 | cortex-m0plus | cortex-m3 | cortex-m4 | cortex-m7
                        (default: host)
  --prefix PATH         Install prefix (default: \$HOME/.local/ucutils_<arch>)
  --build-type TYPE     Debug | Release (default: Debug)
  --examples            Enable UCUTILS_BUILD_EXAMPLES=ON
  --no-tests            Skip tests even on host
  --linker-script PATH  Linker script for ARM test builds
  -h, --help            Show this help

Examples:
  $(basename "$0")
  $(basename "$0") --arch cortex-m4
  $(basename "$0") --arch cortex-m0plus --prefix /opt/custom
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --arch)
      ARCH="$2"
      shift 2
      ;;
    --prefix)
      PREFIX="$2"
      shift 2
      ;;
    --build-type)
      BUILD_TYPE="$2"
      shift 2
      ;;
    --examples)
      BUILD_EXAMPLES="ON"
      shift
      ;;
    --no-tests)
      RUN_TESTS="off"
      shift
      ;;
    --linker-script)
      LINKER_SCRIPT="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

resolve_arch_preset() {
  ARM_CPU=""
  ARM_FPU=""
  ARM_FLOAT_ABI=""
  PREFIX_SUFFIX=""

  case "$ARCH" in
    host)
      PREFIX_SUFFIX="host"
      ;;
    cortex-m0)
      ARM_CPU="cortex-m0"
      ARM_FLOAT_ABI="soft"
      PREFIX_SUFFIX="arm_m0"
      ;;
    cortex-m0plus)
      ARM_CPU="cortex-m0plus"
      ARM_FLOAT_ABI="soft"
      PREFIX_SUFFIX="arm_m0plus"
      ;;
    cortex-m3)
      ARM_CPU="cortex-m3"
      ARM_FLOAT_ABI="soft"
      PREFIX_SUFFIX="arm_m3"
      ;;
    cortex-m4)
      ARM_CPU="cortex-m4"
      ARM_FPU="fpv4-sp-d16"
      ARM_FLOAT_ABI="hard"
      PREFIX_SUFFIX="arm_m4"
      ;;
    cortex-m7)
      ARM_CPU="cortex-m7"
      ARM_FPU="fpv5-sp-d16"
      ARM_FLOAT_ABI="hard"
      PREFIX_SUFFIX="arm_m7"
      ;;
    *)
      echo "Unsupported architecture: $ARCH" >&2
      echo "Supported: host, cortex-m0, cortex-m0plus, cortex-m3, cortex-m4, cortex-m7" >&2
      exit 1
      ;;
  esac
}

resolve_arch_preset

if [[ -z "$PREFIX" ]]; then
  PREFIX="${HOME}/.local/ucutils_${PREFIX_SUFFIX}"
fi

BUILD_DIR="${ROOT}/build/${ARCH}"

CMAKE_ARGS=(
  -S "${ROOT}"
  -B "${BUILD_DIR}"
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
  -DUCUTILS_BUILD_EXAMPLES="${BUILD_EXAMPLES}"
)

if [[ "$ARCH" == "host" ]]; then
  if [[ "$RUN_TESTS" == "auto" ]]; then
    RUN_TESTS="on"
  fi
  CMAKE_ARGS+=(-DUCUTILS_BUILD_TESTS=ON)
else
  if [[ ! -f "$TOOLCHAIN_FILE" ]]; then
    echo "Toolchain file not found: $TOOLCHAIN_FILE" >&2
    exit 1
  fi

  CMAKE_ARGS+=(
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}"
    -DARM_CPU="${ARM_CPU}"
  )

  if [[ -n "$ARM_FPU" ]]; then
    CMAKE_ARGS+=(-DARM_FPU="${ARM_FPU}")
  fi

  if [[ -n "$ARM_FLOAT_ABI" ]]; then
    CMAKE_ARGS+=(-DARM_FLOAT_ABI="${ARM_FLOAT_ABI}")
  fi

  if [[ -n "$LINKER_SCRIPT" ]]; then
    CMAKE_ARGS+=(-DARM_LINKER_SCRIPT="${LINKER_SCRIPT}")
  fi

  if [[ "$RUN_TESTS" == "auto" ]]; then
    if [[ -n "$LINKER_SCRIPT" ]]; then
      RUN_TESTS="on"
      CMAKE_ARGS+=(-DUCUTILS_BUILD_TESTS=ON)
    else
      RUN_TESTS="off"
      CMAKE_ARGS+=(-DUCUTILS_BUILD_TESTS=OFF)
    fi
  elif [[ "$RUN_TESTS" == "on" ]]; then
    if [[ -z "$LINKER_SCRIPT" ]]; then
      echo "ARM test builds require --linker-script PATH" >&2
      exit 1
    fi
    CMAKE_ARGS+=(-DUCUTILS_BUILD_TESTS=ON)
  else
    CMAKE_ARGS+=(-DUCUTILS_BUILD_TESTS=OFF)
  fi
fi

if [[ "$ARCH" == "host" && "$RUN_TESTS" == "off" ]]; then
  CMAKE_ARGS+=(-DUCUTILS_BUILD_TESTS=OFF)
fi

echo "==> Architecture : ${ARCH}"
echo "==> Build dir    : ${BUILD_DIR}"
echo "==> Build type   : ${BUILD_TYPE}"
echo "==> Install prefix: ${PREFIX}"
echo "==> Run tests    : ${RUN_TESTS}"

echo "==> Configuring"
cmake "${CMAKE_ARGS[@]}"

echo "==> Building"
cmake --build "${BUILD_DIR}"

if [[ "$RUN_TESTS" == "on" ]]; then
  echo "==> Testing"
  ctest --test-dir "${BUILD_DIR}" --output-on-failure
else
  echo "==> Skipping tests"
fi

echo "==> Installing"
cmake --install "${BUILD_DIR}" --prefix "${PREFIX}"

cat <<EOF

Done.
  Architecture : ${ARCH}
  Build dir    : ${BUILD_DIR}
  Install prefix: ${PREFIX}

Use in consumer CMake projects:
  cmake -DCMAKE_PREFIX_PATH="${PREFIX}" ...

EOF
