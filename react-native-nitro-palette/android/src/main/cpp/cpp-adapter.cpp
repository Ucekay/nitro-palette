#include <jni.h>
#include "NitroPaletteOnLoad.hpp"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return margelo::nitro::nitropalette::initialize(vm);
}
