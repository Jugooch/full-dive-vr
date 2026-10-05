"""Biosignal acquisition. Sources yield (timestamp, values) samples; they know nothing about intent."""

from .sources import Sample, SerialEMGSource, SimulatedEMGSource

__all__ = ["Sample", "SerialEMGSource", "SimulatedEMGSource"]
