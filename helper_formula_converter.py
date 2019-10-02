import re

formula = """
j n r v - i o r v - j m s v + h o s v + i m t v - h n t v
"""

clean_formula = re.sub(r"\s", "", formula)
normalized_formula = '*'.join(clean_formula)\
    .replace('*-', '-') \
    .replace('*+', '+') \
    .replace('*/', '/') \
    .replace('*(', '(') \
    .replace('*)', ')')\
    .replace('-*', '-') \
    .replace('+*', '+') \
    .replace('/*', '/') \
    .replace('(*', '(') \
    .replace(')*', ')')


def _convert(c):
    if c in ["+", "-", "*", "/", "(", ")", ",", "{", "}"]:
        return c
    return f"m[{ord(c) - ord('a')}]"


print(''.join([_convert(x) for x in normalized_formula]))
