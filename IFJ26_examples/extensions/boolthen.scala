// Rozsireni BOOLTHEN
import ifj26.*

def negate(value: Boolean): Boolean = !value

@main def boolthenExample(): Unit =
  val enabled = readBoolean()
  var divisor = 0
  if enabled then
    divisor = 2
  else
    ()
  val safe = divisor != 0 && (10 / divisor > 1)
  val inverted = negate(enabled)
  val either = enabled || inverted
  print("safe: ")
  print(safe)
  print("\neither: ")
  print(either)
  print("\n")
  if safe then
    print("ready\n")
  else if enabled then
    print("blocked\n")
  else
    print("disabled\n")
  if !enabled then  // podpora podminek bez vetve else
    print("input was false\n")
