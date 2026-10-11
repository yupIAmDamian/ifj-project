package ifj26

// Keep CR here: numeric and Boolean READ reject it rather than trimming it.
private def readInputLine(): String =
  val sb = new StringBuilder
  var c = System.in.read()
  while c != -1 && c != '\n' do
    sb.append(c.toChar)
    c = System.in.read()
  if c == '\n' then
    sb.append('\n')
  sb.toString

def readLine(): String = readInputLine().replace("\r", "")

private def readValueLine(): String = readInputLine().stripSuffix("\n")

def readBooleanMaybe(): Boolean | Unit =
  readValueLine() match
    case "true" => true
    case "false" => false
    case _ => ()

def readBoolean(): Boolean =
  readValueLine() match
    case "true" => true
    case _ => false

def readIntMaybe(): Int | Unit =
  val line = readValueLine()
  if line.matches("[+-]?[0-9]+") then line.toIntOption.getOrElse(())
  else ()

def readInt(): Int =
  readIntMaybe() match
    case value: Int => value
    case _ => 0

private val decimalDoublePattern =
  "[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?[0-9]+)?)"

private val hexDoublePattern =
  "[+-]?0[xX](?:[0-9a-fA-F]+(?:\\.[0-9a-fA-F]*)?|\\.[0-9a-fA-F]+)(?:[pP][+-]?[0-9]+)?"

def readDoubleMaybe(): Double | Unit =
  val line = readValueLine()
  val parsed =
    if line.matches(decimalDoublePattern) then line.toDoubleOption
    else if line.matches(hexDoublePattern) then
      // C strtod accepts a missing binary exponent; Java requires one.
      val literal = if line.exists(c => c == 'p' || c == 'P') then line else line + "p0"
      literal.toDoubleOption
    else None
  parsed.filter(_.isFinite).getOrElse(())

def readDouble(): Double =
  readDoubleMaybe() match
    case value: Double => value
    case _ => 0.0

// C %a spelling: omit an empty fractional part and include the exponent sign.
private def hexDouble(value: Double): String =
  val negative = java.lang.Double.doubleToRawLongBits(value) < 0
  if value.isNaN then
    if negative then "-nan" else "nan"
  else if value.isInfinity then
    if negative then "-inf" else "inf"
  else
    val hex = java.lang.Double.toHexString(value)
    val exponentAt = hex.indexOf('p')
    val mantissa = hex.substring(0, exponentAt).stripSuffix(".0")
    val exponent = hex.substring(exponentAt + 1)
    mantissa + "p" + (if exponent.startsWith("-") then exponent else "+" + exponent)

private def fixedDouble(value: Double): String =
  // Construct from the actual binary value: decimal valueOf/Formatter can round
  // differently from C %.2f at halfway points. Keep the sign of rounded zero.
  val rounded = new java.math.BigDecimal(value).setScale(2, java.math.RoundingMode.HALF_EVEN)
  val sign = if java.lang.Double.doubleToRawLongBits(value) < 0 && rounded.signum() == 0 then "-" else ""
  sign + rounded.toPlainString

def toStr(value: Any): String = value match
  case number: Double if number.isFinite => fixedDouble(number)
  case number: Double => hexDouble(number)
  case values: List[?] => values.iterator.map(toStr).mkString("List(", ", ", ")")
  case values: scala.List[?] => values.iterator.map(toStr).mkString("List(", ", ", ")")
  case _ => value.toString

def print(value: Any): Unit =
  val text = value match
    case number: Double => hexDouble(number)
    case _ => toStr(value)
  scala.Console.print(text)

def toInt(value: Double): Int = value.toInt

def strLength(value: String): Int = value.length

def subStr(value: String, start: Int, end: Int): String =
  if start < 0 || end < 0 || start > end || start >= value.length || end > value.length then
    scala.sys.exit(40)
  value.substring(start, end)

def ord(value: String, index: Int): Int =
  if index < 0 || index >= value.length then -1
  else value.charAt(index).toInt

def chr(code: Int): String =
  if code < 0 || code > 127 then scala.sys.exit(58)
  code.toChar.toString

// Scala's immutable List has the right operations, but its Double formatting
// and exception exit codes differ. Keep its storage behind the IFJ26 interface.
final class List[+A] private[ifj26] (private val values: scala.List[A]):
  def iterator: Iterator[A] = values.iterator

  def apply(index: Int): A =
    if index < 0 || index >= values.length then scala.sys.exit(40)
    values(index)

  def head: A =
    if values.isEmpty then scala.sys.exit(40)
    values.head

  def tail: List[A] =
    if values.isEmpty then scala.sys.exit(40)
    new List(values.tail)

  def drop(count: Int): List[A] = new List(values.drop(count))
  def take(count: Int): List[A] = new List(values.take(count))
  def slice(from: Int, until: Int): List[A] = new List(values.slice(from, until))
  def ++[B >: A](other: List[B]): List[B] = new List(values ++ other.values)
  def filter(predicate: A => Boolean): List[A] = new List(values.filter(predicate))
  def takeWhile(predicate: A => Boolean): List[A] = new List(values.takeWhile(predicate))
  def dropWhile(predicate: A => Boolean): List[A] = new List(values.dropWhile(predicate))
  def foreach[U](function: A => U): Unit = values.foreach(function)
  def withFilter(predicate: A => Boolean): Guarded[A] = new Guarded(() => iterator.filter(predicate))

  override def toString: String = toStr(this)
  override def equals(other: Any): Boolean = other match
    case list: List[?] => values == list.values
    case _ => false
  override def hashCode(): Int = values.hashCode()

object List:
  def apply[A](values: A*): List[A] = new List(values.toList)
  def range(start: Int, end: Int): List[Int] = range(start, end, 1)
  def range(start: Int, end: Int, step: Int): List[Int] =
    if step == 0 then scala.sys.exit(40)
    new List(scala.List.range(start, end, step))

// Guards stay lazy and run in order for each element, just as in Scala's for.
final class Guarded[+A] private[ifj26] (private val source: () => Iterator[A]):
  def foreach[U](function: A => U): Unit = source().foreach(function)
  def withFilter(predicate: A => Boolean): Guarded[A] = new Guarded(() => source().filter(predicate))

final class IntRange private[ifj26] (private val values: scala.collection.immutable.Range):
  infix def by(step: Int): IntRange =
    if step == 0 then scala.sys.exit(40)
    new IntRange(values.by(step))
  def foreach[U](function: Int => U): Unit = values.foreach(function)
  def withFilter(predicate: Int => Boolean): Guarded[Int] = new Guarded(() => values.iterator.filter(predicate))
  def toList: List[Int] = new List(values.toList)

// Imported extensions take precedence over the standard RichInt conversion.
extension (start: Int)
  infix def to(end: Int): IntRange = new IntRange(scala.collection.immutable.Range.inclusive(start, end))
  infix def until(end: Int): IntRange = new IntRange(scala.collection.immutable.Range(start, end))
