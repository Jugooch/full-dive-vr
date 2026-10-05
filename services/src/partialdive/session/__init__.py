"""Session management: every run gets traceable metadata and a programmatically randomized,
blinded condition order. Never pick conditions by hand."""

from .randomize import Block, blinded_order
from .session import init_session, load_conditions, unblind_session

__all__ = ["Block", "blinded_order", "init_session", "load_conditions", "unblind_session"]
